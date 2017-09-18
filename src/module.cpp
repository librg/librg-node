#include "stdafx.h"

namespace node_librg
{

	v8::Local<v8::Object> FormatEntityObject(v8::Isolate* isolate, librg_entity_t entityId)
	{
		v8::Local<v8::Object> obj = v8::Object::New(isolate);
		obj->Set(v8::String::NewFromUtf8(isolate, "id"), v8::Number::New(isolate, entityId));

		librg_transform_t* transform = librg_fetch_transform(entityId);
		v8::Local<v8::Object> pos = v8::Object::New(isolate);
		pos->Set(v8::String::NewFromUtf8(isolate, "x"), v8::Number::New(isolate, transform->position.x));
		pos->Set(v8::String::NewFromUtf8(isolate, "y"), v8::Number::New(isolate, transform->position.y));
		pos->Set(v8::String::NewFromUtf8(isolate, "z"), v8::Number::New(isolate, transform->position.z));
		obj->Set(v8::String::NewFromUtf8(isolate, "position"), pos);

		v8::Local<v8::Object> rot = v8::Object::New(isolate);
		rot->Set(v8::String::NewFromUtf8(isolate, "x"), v8::Number::New(isolate, transform->rotation.x));
		rot->Set(v8::String::NewFromUtf8(isolate, "y"), v8::Number::New(isolate, transform->rotation.y));
		rot->Set(v8::String::NewFromUtf8(isolate, "z"), v8::Number::New(isolate, transform->rotation.z));
		rot->Set(v8::String::NewFromUtf8(isolate, "w"), v8::Number::New(isolate, transform->rotation.w));
		obj->Set(v8::String::NewFromUtf8(isolate, "rotation"), rot);

		u32 type = librg_entity_type(entityId);
		obj->Set(v8::String::NewFromUtf8(isolate, "type"), v8::Number::New(isolate, type));
		return obj;
	}

	v8::Local<v8::Object> FormatEventObject(v8::Isolate *isolate, librg_event_t *event)
	{
		v8::Local<v8::Object> obj = v8::Object::New(isolate);
		//TODO: Data
		obj->Set(v8::String::NewFromUtf8(isolate, "entity"), FormatEntityObject(isolate, event->entity));
		obj->Set(v8::String::NewFromUtf8(isolate, "reject"), v8::Boolean::New(isolate, false));
		return obj;
	}

	void CallEventFunction(v8::Isolate* isolate, librg_event_t* event, v8::Persistent<v8::Function> *func)
	{
		v8::Local<v8::Context> context = isolate->GetCurrentContext();
		v8::Local<v8::Function> callbackFunc = v8::Local<v8::Function>::New(isolate, *func);
		v8::Local<v8::Object> eventObject = FormatEventObject(isolate, event);
		v8::Local<v8::Value> args[] = { eventObject };
		callbackFunc->Call(context->Global(), 1, args);
		if (eventObject->Get(v8::String::NewFromUtf8(isolate, "reject"))->BooleanValue() == true)
		{
			librg_event_reject(event);
		}
	}

	void OnConnectionRequest(librg_event_t *event)
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		v8::Local<v8::Context> context = isolate->GetCurrentContext();
		for (auto const& e : m_events)
		{
			if (e->eventId == LIBRG_CONNECTION_REQUEST)
			{
				CallEventFunction(isolate, event, e->func);
			}
		}
	}

	void OnConnectionAccept(librg_event_t *event)
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		for (auto const& e : m_events)
		{
			if (e->eventId == LIBRG_CONNECTION_ACCEPT)
			{
				CallEventFunction(isolate, event, e->func);
			}
		}
	}

	void OnEntityCreate(librg_event_t *event)
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		v8::Local<v8::Context> context = isolate->GetCurrentContext();
		for (auto const& e : m_events)
		{
			if (e->eventId == LIBRG_ENTITY_CREATE)
			{
				CallEventFunction(isolate, event, e->func);
			}
		}
	}

	void OnEntityUpdate(librg_event_t *event)
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		v8::Local<v8::Context> context = isolate->GetCurrentContext();
		for (auto const& e : m_events)
		{
			if (e->eventId == LIBRG_ENTITY_UPDATE)
			{
				CallEventFunction(isolate, event, e->func);
			}
		}
	}

	void OnEntityRemove(librg_event_t *event)
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		v8::Local<v8::Context> context = isolate->GetCurrentContext();
		for (auto const& e : m_events)
		{
			if (e->eventId == LIBRG_ENTITY_REMOVE)
			{
				CallEventFunction(isolate, event, e->func);
			}
		}
	}

	void InitMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		if (args.Length() < 1)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		if (args[0]->IsObject() == false)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "First argument is not object")));
			return;
		}

		v8::Local<v8::Object> options = args[0]->ToObject();

		librg_config_t config = { 0 };

		if (options->Has(v8::String::NewFromUtf8(isolate, "maxConnections")))
			config.max_connections = options->Get(v8::String::NewFromUtf8(isolate, "maxConnections"))->NumberValue();
		if (options->Has(v8::String::NewFromUtf8(isolate, "maxEntities")))
			config.max_entities = options->Get(v8::String::NewFromUtf8(isolate, "maxEntities"))->NumberValue();
		if (options->Has(v8::String::NewFromUtf8(isolate, "mode"))) {
			std::string mode(*v8::String::Utf8Value(options->Get(v8::String::NewFromUtf8(isolate, "mode"))->ToString()));
			if (mode == "server") {
				config.mode = LIBRG_MODE_SERVER;
			}
			else if (mode == "client")
			{
				config.mode = LIBRG_MODE_CLIENT;
			}

		}
		if (options->Has(v8::String::NewFromUtf8(isolate, "tickDelay")))
			config.tick_delay = options->Get(v8::String::NewFromUtf8(isolate, "tickDelay"))->NumberValue();
		if (options->Has(v8::String::NewFromUtf8(isolate, "worldSize")))
		{
			v8::Local<v8::Array> floatArray = v8::Local<v8::Array>::Cast(options->Get(v8::String::NewFromUtf8(isolate, "worldSize")));
			config.world_size = zplm_vec2(floatArray->Get(0)->NumberValue(), floatArray->Get(1)->NumberValue());
		}

		m_events = std::vector<EventStruct*>();

		librg_init(config);
		librg_event_add(LIBRG_CONNECTION_REQUEST, OnConnectionRequest);
		librg_event_add(LIBRG_CONNECTION_ACCEPT, OnConnectionAccept);
		librg_event_add(LIBRG_ENTITY_CREATE, OnEntityCreate);
		librg_event_add(LIBRG_ENTITY_UPDATE, OnEntityUpdate);
		librg_event_add(LIBRG_ENTITY_REMOVE, OnEntityRemove);
	}

	void StartMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		if (args.Length() < 1)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		if (args[0]->IsObject() == false)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "First argument is not object")));
			return;
		}

		v8::Local<v8::Object> options = args[0]->ToObject();

		librg_address_t address = { 0 };

		if (options->Has(v8::String::NewFromUtf8(isolate, "port")))
			address.port = options->Get(v8::String::NewFromUtf8(isolate, "port"))->NumberValue();
		if (options->Has(v8::String::NewFromUtf8(isolate, "address"))) {
			address.host = (*v8::String::Utf8Value(options->Get(v8::String::NewFromUtf8(isolate, "address"))->ToString()));
		}

		
		librg_network_start(address);
	}

	void TickMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		librg_tick();
	}

	void StopMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		librg_network_stop();
	}

	void DisposeMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		for (auto const& e : m_events)
		{
			delete e->func;
		}
		delete &m_events;
		librg_free();
	}

	void OnMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		if (args.Length() < 2)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		if (args[0]->IsString() == false)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "First argument is not string")));
			return;
		}

		if (args[1]->IsFunction() == false)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Second argument is not function")));
			return;
		}

		std::string eventName = std::string(*v8::String::Utf8Value(args[0]->ToString()));
		//v8::Persistent<v8::Function>* callback = new v8::Persistent<v8::Function>();
		//callback->Reset(isolate, args[1].As<v8::Function>());
		v8::Persistent<v8::Function>* callback = new v8::Persistent<v8::Function>(isolate, v8::Handle<v8::Function>::Cast(args[1]));

		EventStruct* es = new EventStruct();
		if (eventName == "connectionRequest")
			es->eventId = LIBRG_CONNECTION_REQUEST;
		else if (eventName == "connectionAccept")
			es->eventId = LIBRG_CONNECTION_ACCEPT;
		else if (eventName == "entityCreate")
			es->eventId = LIBRG_ENTITY_CREATE;
		else if (eventName == "entityUpdate")
			es->eventId = LIBRG_ENTITY_UPDATE;
		else if (eventName == "entityRemove")
			es->eventId = LIBRG_ENTITY_REMOVE;
		es->func = callback;
		m_events.push_back(es);
	}

	void Init(v8::Local<v8::Object> exports) {
		NODE_SET_METHOD(exports, "initialize", InitMethod);
		NODE_SET_METHOD(exports, "start", StartMethod);
		NODE_SET_METHOD(exports, "tick", TickMethod);
		NODE_SET_METHOD(exports, "stop", StopMethod);
		NODE_SET_METHOD(exports, "dispose", DisposeMethod);
		NODE_SET_METHOD(exports, "on", OnMethod);
	}

	NODE_MODULE(librg_node, Init)

}