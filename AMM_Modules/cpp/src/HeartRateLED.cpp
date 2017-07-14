#include "stdafx.h"

#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"
#include <iostream>
#include <iomanip>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

using namespace DDS;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears; // ?


static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1<<23;
static uint16_t delay;

int
spi_transfer(int fd, unsigned char *tx_buf, unsigned char *rx_buf, int buflen)
{
	int ret;
	struct spi_ioc_transfer tr = {
		tx_buf : (unsigned long)tx_buf,
		rx_buf : (unsigned long)rx_buf,
		len : buflen,
		speed_hz : speed,
		delay_usecs : delay,
		bits_per_word : bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		perror("can't send spi message");
	return ret;
}

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	// usage : VirtualEquipment <subscription_string>
	const char *nodePath = "Respiratory_Respiration_Rate";//"HR";
	os_time delay_200ms = { 0, 200000000 };
	char buf[MAX_MSG_LEN];
	char topicName[] = "Command";
	int spi_fd = open(device, O_RDWR);

	NodeSeq msgList;
	SampleInfoSeq infoSeq;

	DDSEntityManager mgr;
	DDSEntityManager mgrcmd;

	// create domain participant
	char partition_name[] = "AMM";
	mgr.createParticipant(partition_name);
	mgrcmd.createParticipant(partition_name);

	//create type
	NodeTypeSupport_var dt = new NodeTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	char topic_name[] = "Data";
	mgr.createTopic(topic_name);
	//create Subscriber
	mgr.createSubscriber();

	char sTopicName[] = "MyDataTopic";
	// create subscription filter
	snprintf(buf, MAX_MSG_LEN, "nodepath = '%s'",
			nodePath);
	DDS::String_var sFilter = DDS::string_dup(buf);

	// Filter expression
	StringSeq sSeqExpr;
	sSeqExpr.length(0);
	// create topic
	cout << sTopicName << " " << sFilter << " " << endl;
	mgr.createContentFilteredTopic(sTopicName, sFilter.in(), sSeqExpr);
	// create Filtered DataReader
	cout << "=== [VirtualEquipment] Subscription filter : "
			<< sFilter << endl;
	mgr.createReader(true);

	DataReader_var dreader = mgr.getReader();
	NodeDataReader_var PhysiologyDataReader = NodeDataReader::_narrow(
			dreader.in());
	checkHandle(PhysiologyDataReader.in(), "NodeDataReader::_narrow");
	
	//make command writer
	CommandTypeSupport_var cdt = new CommandTypeSupport();
	mgrcmd.registerType(cdt.in());
	mgrcmd.createTopic(topicName);
	mgrcmd.createPublisher();
	mgrcmd.createWriter();

	// Publish Events
	DataWriter_var dwriter = mgrcmd.getWriter();
	CommandDataWriter_var CommandWriter = CommandDataWriter::_narrow(
			dwriter.in());
	checkHandle(CommandWriter.in(), "CommandDataWriter::_narrow");

	cout << "=== [VirtualEquipment] Ready ..." << endl;

	float heartrate = 60.0;
	bool closed = false;
	ReturnCode_t status = -1;
	int count = 0;
	while (!closed)
	{
		status = PhysiologyDataReader->take(msgList, infoSeq,
				LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE,
				ANY_INSTANCE_STATE);
		checkStatus(status, "NodeDataReader::take");
		for (DDS::ULong i = 0; i < msgList.length(); i++) {
			if (infoSeq[i].valid_data) {
				if (msgList[i].dbl == -1.0f) {
					closed = true;
					break;
				}
				heartrate = msgList[i].dbl;
			}
			cout
					<< "=== [VirtualEquipment] Received data :  ("
					<< msgList[i].nodepath << ", " << msgList[i].dbl << ')'
					<< endl;
		}
		//prepare SPI message
		/*
			heartrate = 60 (Example)
			heartrate/60 = 1 = beats/second
			seconds/ms = 1/1000
			want ms/beat
			beats/second*seconds/ms = beats/ms
			1/beats/ms = ms/beat
			
			answer = 1/(beats/min * min/sec * sec/ms)
			answer = 1/(hr * (1/60) * 0.001)
		*/
		//int spi_msg_full = 1.0/(heartrate * (1.0/60.0) * 0.001);
		unsigned char spi_send[4];
		spi_send[0] = heartrate;
		//TODO spi_send[1] = tourniquet sent?
		unsigned char spi_rcvd[4];
		
		//do SPI communication
		int spi_tr_res = spi_transfer(spi_fd, spi_send, spi_rcvd, 4);
		
		//std::cout << "spi_msg " << std::hex << std::setw(2)
		//	<< std::setfill('0') << (unsigned int) spi_msg << std::endl;
		//std::cout << "spi_rcvd " << std::hex << std::setw(2)
		//	<< std::setfill('0') << (unsigned int) spi_rcvd << std::endl;
		//send press messages based on received SPI
		//the buttons send 1 when they are up and 0 when they are pressed
		if (spi_rcvd[1]) {
			//button 2 was pressed
			//send hemorrhage action
			Command cmdInstance;
			cmdInstance.message = DDS::string_dup("LEG_HEMORRHAGE");
			cout << "=== [CommandExecutor] Sending a command containing:" << endl;
			cout << "    Command : \"" << cmdInstance.message << "\"" << endl;
			status = CommandWriter->write(cmdInstance, DDS::HANDLE_NIL);
			checkStatus(status, "CommandWriter::write");
			cout << "sent that command" << endl;
		}
		if (spi_rcvd[2]) {
			//button 1 was pressed
			//send tourniquet action
			Command cmdInstance;
			cmdInstance.message = DDS::string_dup("START");
			cout << "=== [CommandExecutor] Sending a command containing:" << endl;
			cout << "    Command : \"" << cmdInstance.message << "\"" << endl;
			status = CommandWriter->write(cmdInstance, DDS::HANDLE_NIL);
			checkStatus(status, "CommandWriter::write");
		}

		status = PhysiologyDataReader->return_loan(msgList, infoSeq);
		checkStatus(status, "DataDataReader::return_loan");
		os_nanoSleep(delay_200ms);
		++count;
	}

	cout << "=== [VirtualEquipment] Simulation stopped."
			<< endl;

	//cleanup
	mgr.deleteReader(PhysiologyDataReader.in());
	mgr.deleteSubscriber();
	mgr.deleteFilteredTopic();
	mgr.deleteTopic();
	mgr.deleteParticipant();
	mgrcmd.deleteWriter(CommandWriter.in());
	mgrcmd.deletePublisher();
	mgrcmd.deleteTopic();
	mgrcmd.deleteParticipant();


	return 0;

}
