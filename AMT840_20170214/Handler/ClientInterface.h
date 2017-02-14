#pragma once


// CClientInterface

class CClientInterface
{

public:
	CClientInterface();
	virtual ~CClientInterface();


public:
	void					OnCommunication();
};
extern CClientInterface clsInterface;

