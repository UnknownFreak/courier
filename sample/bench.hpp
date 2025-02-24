#pragma once

namespace bench
{
	void run();


	struct msgStat
	{
		long long dur = 0;
		long long msgcount = 999999999999;
		long long avgCount = 0;

		void set(long long d, long long msgC)
		{
			dur = d;
			msgcount = msgC;
			avgCount = msgcount / dur;
		}

		void add(long long d, long long msgC)
		{
			dur += d;
			msgcount += msgC;
			avgCount = msgcount / dur;
		}
	};
}
