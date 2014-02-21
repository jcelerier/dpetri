#pragma once
#include <iostream>
#include <thread>
#include <fstream>
#include <pnapi/pnapi.h>
#include <climits>
using namespace std;
using namespace pnapi;

void basicPetriNet()
{
	//Clock c;
	PetriNet net;

	auto& i = net.createPlace("init");
	auto& f = net.createPlace("final");
	i.setTokenCount(1);
	auto& p1 = net.createPlace();
	auto& p2 = net.createPlace();
	auto& p3 = net.createPlace();
	auto& p4 = net.createPlace();
	auto& p5 = net.createPlace();
	auto& p6 = net.createPlace();
	auto& p7 = net.createPlace();
	auto& p8 = net.createPlace();
	auto& p9 = net.createPlace();

	auto& t1 = net.createTransition();
	t1.setCost(800);
	auto& t2 = net.createTransition();
	t2.setCost(900);
	auto& t3 = net.createTransition();
	t3.setCost(1600);
	auto& t4 = net.createTransition();
	t4.setCost(2400);
	auto& t5 = net.createTransition();
	t5.setCost(2200);
	auto& t6 = net.createTransition();
	t6.setCost(3000);
	auto& t7 = net.createTransition();
	t7.setCost(3600);
	auto& t8 = net.createTransition();
	t8.setCost(5000);

	net.createArc(i, t1);
	net.createArc(t1, p1);
	net.createArc(t1, p2);

	net.createArc(p1, t2);
	net.createArc(t2, p3);
	net.createArc(p3, t4);
	net.createArc(t4, p6);

	net.createArc(p2, t3);
	net.createArc(t3, p4);
	net.createArc(t3, p5);

	net.createArc(p4, t5);
	net.createArc(t5, p6);

	net.createArc(p5, t6);
	net.createArc(t6, p7);

	net.createArc(p6, t7);
	net.createArc(p7, t7);

	net.createArc(t7, p8);
	net.createArc(t7, p9);
	net.createArc(p8, t8);
	net.createArc(p9, t8);

	net.createArc(t8, f);

	std::ofstream outfile("base.fiona");
	outfile << io::owfn << net;

//	std::thread t(&Clock::run, &c);

//	test(net, c);
//	t.join();
}
