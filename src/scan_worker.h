#ifndef __SCAN_WORKER_H__
#define __SCAN_WORKER_H__

#include <nan.h>

#include "scan.h"

class ScanWorker : public Nan::AsyncWorker {
public:
	ScanWorker(Nan::Callback *callback, std::string path, ScanConfig config, std::vector<double> bubles);
	~ScanWorker() {};

	void Execute();
	void HandleOKCallback();

private:
	std::string m_ImagePath;
	ScanConfig m_Config;
	std::vector<double> m_Bubbles;

	std::vector<double> m_Output;
};

#endif
