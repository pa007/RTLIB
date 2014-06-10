#include "catch.hpp"
#include <rttask.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("CBS algorithm: Original")
{
    PeriodicTask t1(8, 8, 0, "TaskA");
    t1.insertCode("fixed(2);");
    t1.setAbort(false);

    PeriodicTask t2(10, 10, 0, "TaskB");
    t2.insertCode("fixed(3);");
    t2.setAbort(false);

    EDFScheduler sched;
    RTKernel kern(&sched);
    
    CBServer serv(5, 15, 15, true,  "server1", "FIFOSched");
    serv.addTask(t2);

    kern.addTask(t1);
    kern.addTask(serv);

    SECTION("Original") {
	serv.set_policy(CBServer::ORIGINAL);
	SIMUL.initSingleRun();
	
	SIMUL.run_to(3);
	
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 1);
	REQUIRE(serv.get_remaining_budget() == 4);

	SIMUL.run_to(5);
	REQUIRE(t2.getExecTime() == 3);
	REQUIRE(serv.get_remaining_budget() == 2);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(8);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(9);
	REQUIRE(serv.getStatus() == IDLE);
	SIMUL.run_to(10);
	REQUIRE(serv.getStatus() == EXECUTING);
	REQUIRE(serv.getDeadline() == 25);

	SIMUL.endSingleRun();
    }

    SECTION("Reuse dline") {
	serv.set_policy(CBServer::REUSE_DLINE);
	SIMUL.initSingleRun();
	
	SIMUL.run_to(3);
	
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 1);
	REQUIRE(serv.get_remaining_budget() == 4);	

	SIMUL.run_to(5);
	REQUIRE(t2.getExecTime() == 3);
	REQUIRE(serv.get_remaining_budget() == 2);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(8);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(9);
	REQUIRE(serv.getStatus() == IDLE);
	SIMUL.run_to(10);
	REQUIRE(serv.getStatus() == EXECUTING);
	REQUIRE(serv.getDeadline() == 15);
	REQUIRE(serv.get_remaining_budget() == 1);

	SIMUL.run_to(11);
	REQUIRE(serv.getStatus() == RECHARGING);
	REQUIRE(serv.getDeadline() == 30);
	
	SIMUL.endSingleRun();
    }
}
