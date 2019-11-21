#pragma once
class MSTimer
{
public:
	MSTimer();
	~MSTimer();
	void Start();
	unsigned int Read();
	unsigned int Stop();

private:
	unsigned int currentTime;
	unsigned int startingTime;
	bool run;
};

