#include <nan.h>

#include "scan.h"
#include "scan_worker.h"

NAN_METHOD(Scan) {
	ScanConfig config;
	std::vector<double> bubbles;

	v8::String::Utf8Value image_path(info[0]);
	v8::Handle<v8::Array> config_array = v8::Handle<v8::Array>::Cast(info[1]);
	v8::Handle<v8::Array> bubble_array = v8::Handle<v8::Array>::Cast(info[2]);

	config.square_width = Nan::To<double>(config_array->Get(0)).FromJust();
	config.square_height = Nan::To<double>(config_array->Get(1)).FromJust();
	config.circle_width = Nan::To<double>(config_array->Get(2)).FromJust();
	config.circle_height = Nan::To<double>(config_array->Get(3)).FromJust();

	for (size_t i = 0; i < bubble_array->Length(); ++i) {
		bubbles.push_back(Nan::To<double>(bubble_array->Get(i)).FromJust());
	}

	Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
	Nan::AsyncQueueWorker(new ScanWorker(callback, std::string(*image_path), config, bubbles));
}

NAN_MODULE_INIT(InitAll) {
	Nan::Set(target, Nan::New<v8::String>("scan").ToLocalChecked(),
			 Nan::GetFunction(Nan::New<v8::FunctionTemplate>(Scan)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
