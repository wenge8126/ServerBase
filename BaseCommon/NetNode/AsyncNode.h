#ifndef _INCLUDE_ASYNCNODE_H_
#define _INCLUDE_ASYNCNODE_H_

#pragma once


class AsyncNode : public tNetProcess
{


public:
	Hand<MeshNet>		mNodeNet;
};


#endif //_INCLUDE_ASYNCNODE_H_
