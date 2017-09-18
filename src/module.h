#pragma once

struct EventStruct
{
	u64 eventId;
	v8::Persistent<v8::Function>* func;
};

std::vector<EventStruct*> m_events;
