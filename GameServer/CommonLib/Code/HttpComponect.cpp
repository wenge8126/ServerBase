#include "HttpComponect.h"



void HttpComponect::Start()
{
	if (mCertFile.length() > 0)
		mHttpNet = MEM_NEW CompHttpsWebNet<true>(this);
	else
		mHttpNet = MEM_NEW CompHttpsWebNet<false>(this);
	Hand<BaseWebServer> net = mHttpNet;
	net->StartWeb(mPort, mKeyFile, mCertFile, mPassword);
	
}
