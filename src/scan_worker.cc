#include "scan_worker.h"

ScanWorker::ScanWorker(Nan::Callback *callback, std::string path, ScanConfig config, std::vector<double> bubbles) : Nan::AsyncWorker(callback), m_ImagePath(path), m_Config(config), m_Bubbles(bubbles) {}

void ScanWorker::Execute() {
	m_Output = scan(m_ImagePath, m_Config, m_Bubbles);
}

void ScanWorker::HandleOKCallback() {
	Nan::HandleScope scope;

	v8::Local<v8::Array> output = Nan::New<v8::Array>(m_Output.size());

	for (size_t i = 0; i < m_Output.size(); i++) {
		output->Set(i, Nan::New<v8::Number>(m_Output[i]));
	}

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		output
	};

	callback->Call(2, argv);
}
