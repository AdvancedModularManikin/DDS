#include "stdafx.h"

#include <algorithm>

#include <pistache/http.h>
#include <pistache/http_header.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <mutex>
#include <thread>

#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"

using namespace std;
using namespace rapidjson;
using namespace DDS;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace Pistache;

DDSEntityManager dataMgr;
DDSEntityManager cmdMgr;
DataWriter_var cmddwriter;
CommandDataWriter_var CommandWriter;
DataReader_var dreader;
NodeDataReader_var PhysiologyDataReader;

std::map<std::string, double> nodeDataStorage;

std::thread m_thread;
std::mutex m_mutex;
bool m_runThread = false;

void SendCommand(const std::string &command) {
	Command cmdInstance;
	cmdInstance.message = DDS::string_dup(command.c_str());
	cout << "=== [CommandExecutor] Sending a command containing:" << endl;
	cout << "    Command : \"" << cmdInstance.message << "\"" << endl;
	CommandWriter->write(cmdInstance, DDS::HANDLE_NIL);
}

void InitializeDDS() {
	char partition_name[] = "AMM";
	char node_topic_name[] = "Data";
	char cmd_topic_name[] = "Command";

	// Set up command manager
	DDSEntityManager cmdMgr;
	cmdMgr.createParticipant(partition_name);
	CommandTypeSupport_var dt = new CommandTypeSupport();
	cmdMgr.registerType(dt.in());
	cmdMgr.createTopic(cmd_topic_name);
	cmdMgr.createPublisher();
	cmdMgr.createWriters();
	DataWriter_var cmddwriter = cmdMgr.getWriter();
	CommandWriter = CommandDataWriter::_narrow(cmddwriter.in());

	// Set up data manager
	DDSEntityManager dataMgr;
	dataMgr.createParticipant(partition_name);
	NodeTypeSupport_var ndt = new NodeTypeSupport();
	dataMgr.registerType(ndt.in());
	dataMgr.createTopic(node_topic_name);
	dataMgr.createSubscriber();
	dataMgr.createReader(false);
	DataReader_var dreader = dataMgr.getReader();
	PhysiologyDataReader = NodeDataReader::_narrow(dreader.in());
}

void CleanupDDS() {
	cmdMgr.deleteWriters();
	cmdMgr.deletePublisher();
	cmdMgr.deleteTopic();
	cmdMgr.deleteParticipant();

	dataMgr.deleteReader(PhysiologyDataReader.in());
	dataMgr.deleteSubscriber();
	dataMgr.deleteTopic();
	dataMgr.deleteParticipant();
}

void DataLoop() {
	NodeSeq msgList;
	SampleInfoSeq infoSeq;

	while (m_runThread) {
		PhysiologyDataReader->take(msgList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		for (DDS::ULong i = 0; i < msgList.length(); i++) {
			if (infoSeq[i].valid_data) {
				nodeDataStorage[msgList[i].nodepath.val()] = msgList[i].dbl;
			}
		}
		PhysiologyDataReader->return_loan(msgList, infoSeq);
	}
}

void printCookies(const Http::Request& req) {
	auto cookies = req.cookies();
	std::cout << "Cookies: [" << std::endl;
	const std::string indent(4, ' ');
	for (const auto& c : cookies) {
		std::cout << indent << c.name << " = " << c.value << std::endl;
	}
	std::cout << "]" << std::endl;
}

namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response) {
	response.send(Http::Code::Ok, "1");
}

}

class DDSEndpoint {
public:

	DDSEndpoint(Address addr) :
			httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {
	}

	void init(size_t thr = 2) {
		auto opts = Http::Endpoint::options().threads(thr).flags(Tcp::Options::InstallSignalHandler);
		httpEndpoint->init(opts);
		setupRoutes();
	}

	void start() {
		httpEndpoint->setHandler(router.handler());
		httpEndpoint->serve();
	}

	void shutdown() {
		httpEndpoint->shutdown();
	}

private:
	void setupRoutes() {
		using namespace Rest;

		Routes::Get(router, "/node/:name", Routes::bind(&DDSEndpoint::doGetNode, this));
		Routes::Get(router, "/nodes", Routes::bind(&DDSEndpoint::doGetAllNodes, this));
		Routes::Get(router, "/command/:name", Routes::bind(&DDSEndpoint::doIssueCommand, this));
		Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
		Routes::Get(router, "/debug", Routes::bind(&DDSEndpoint::doDebug, this));

	}

	void doIssueCommand(const Rest::Request& request, Http::ResponseWriter response) {
		auto name = request.param(":name").as<std::string>();
		SendCommand(name);
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("Sent command");
		writer.String(name.c_str());
		writer.EndObject();
		response.headers().addRaw(Http::Header::Raw("Access-Control-Allow-Origin","*"));
		response.send(Http::Code::Ok, s.GetString());
	}

	void doGetAllNodes(const Rest::Request& request, Http::ResponseWriter response) {
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartArray();
		std::map<std::string, double>::iterator it = nodeDataStorage.begin();
		while (it != nodeDataStorage.end()) {
			writer.StartObject();
			writer.Key(it->first.c_str());
			std::ostringstream s;
			s << it->second;
			writer.String(s.str().c_str());
			writer.EndObject();
			it++;
		}
		writer.EndArray();
		// response.headers().add(<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
		response.headers().addRaw(Http::Header::Raw("Access-Control-Allow-Origin","*"));
		response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
	}

	void doGetNode(const Rest::Request& request, Http::ResponseWriter response) {

		auto name = request.param(":name").as<std::string>();
		string responseString = "";
		std::map<std::string, double>::iterator it = nodeDataStorage.find(name);
		if (it != nodeDataStorage.end()) {
			StringBuffer s;
			Writer<StringBuffer> writer(s);
			writer.StartObject();
			writer.Key(it->first.c_str());
			std::ostringstream ns;
			ns << it->second;
			writer.String(ns.str().c_str());
			writer.EndObject();
			response.headers().addRaw(Http::Header::Raw("Access-Control-Allow-Origin","*"));
			response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
		} else {
			response.send(Http::Code::Not_Found, "Node data does not exist");
		}
	}

	void doDebug(const Rest::Request& request, Http::ResponseWriter response) {
		printCookies(request);
		response.cookies().add(Http::Cookie("lang", "en-US"));
		response.send(Http::Code::Ok);
	}

	typedef std::mutex Lock;
	typedef std::lock_guard<Lock> Guard;
	Lock commandLock;

	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Router router;
};

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	int portNumber = 9080;
	int thr = 2;

	InitializeDDS();

	// start data thread
	m_runThread = true;
	m_thread = std::thread(DataLoop);

	Port port(portNumber);
	Address addr(Ipv4::any(), port);
	DDSEndpoint server(addr);
	cout << "=== [REST_Adapter] Ready ..." << endl;
	cout << "  = [REST_Adapter] Listening on *:" << portNumber << endl;
	cout << "  = [REST_Adapter] Cores = " << hardware_concurrency() << endl;
	cout << "  = [REST_Adapter] Using " << thr << " threads" << endl;

	server.init(thr);
	server.start();

	cout << "=== [REST_Adapter] Simulation stopped." << endl;

	server.shutdown();
	m_thread.join();
	CleanupDDS();

	return 0;
}
