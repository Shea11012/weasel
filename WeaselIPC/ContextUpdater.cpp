#include "stdafx.h"
#include <StringAlgorithm.hpp>
#include <WeaselUtility.h>

#include "Deserializer.h"
#include "ContextUpdater.h"

using namespace weasel;

// ContextUpdater

Deserializer::Ptr ContextUpdater::Create(ResponseParser* pTarget)
{
	return Deserializer::Ptr(new ContextUpdater(pTarget));
}

ContextUpdater::ContextUpdater(ResponseParser* pTarget)
	: Deserializer(pTarget)
{
}

ContextUpdater::~ContextUpdater()
{
}

void ContextUpdater::Store(Deserializer::KeyType const& k, std::wstring const& value)
{
	if(!m_pTarget->p_context || k.size() < 2)
		return;

	if (k[1] == L"preedit")
	{
		_StoreText(m_pTarget->p_context->preedit, k, value);
		return;
	}

	if (k[1] == L"aux")
	{
		_StoreText(m_pTarget->p_context->aux, k, value);
		return;
	}

	if (k[1] == L"cand")
	{
		_StoreCand(k, value);
		return;
	}
}

void ContextUpdater::_StoreText(Text& target, Deserializer::KeyType k, std::wstring const& value)
{
	if(k.size() == 2)
	{
		target.clear();
		target.str = value;
		return;
	}
	if(k.size() == 3)
	{
		// ctx.preedit.cursor
		if (k[2] == L"cursor")
		{
			std::vector<std::wstring> vec;
			split(vec, value, L",");
			if (vec.size() < 2)
				return;

			weasel::TextAttribute attr;
			attr.type = HIGHLIGHTED;
			attr.range.start = _wtoi(vec[0].c_str());
			attr.range.end = _wtoi(vec[1].c_str());
			attr.range.cursor = _wtoi(vec[2].c_str());
			if (attr.range.cursor < attr.range.start || attr.range.cursor > attr.range.end)
			{
				attr.range.cursor = attr.range.end;
			}
			
			target.attributes.push_back(attr);
			return;
		}
	}
}

void ContextUpdater::_StoreCand(Deserializer::KeyType k, std::wstring const& value)
{
	CandidateInfo& cinfo = m_pTarget->p_context->cinfo;
	std::wstringstream ss(value);
	boost::archive::text_wiarchive ia(ss);

	ia >> cinfo;
}

// StatusUpdater

Deserializer::Ptr StatusUpdater::Create(ResponseParser* pTarget)
{
	return Deserializer::Ptr(new StatusUpdater(pTarget));
}

StatusUpdater::StatusUpdater(ResponseParser* pTarget)
: Deserializer(pTarget)
{
}

StatusUpdater::~StatusUpdater()
{
}

void StatusUpdater::Store(Deserializer::KeyType const& k, std::wstring const& value)
{
	if(!m_pTarget->p_status || k.size() < 2)
		return;

	bool bool_value = (!value.empty() && value != L"0");

	if (k[1] == L"schema_id")
	{
		m_pTarget->p_status->schema_id = value;
		return;
	}

	if (k[1] == L"ascii_mode")
	{
		m_pTarget->p_status->ascii_mode = bool_value;
		return;
	}

	if (k[1] == L"composing")
	{
		m_pTarget->p_status->composing = bool_value;
		return;
	}

	if (k[1] == L"disabled")
	{
		m_pTarget->p_status->disabled = bool_value;
		return;
	}

	if (k[1] == L"full_shape")
	{
		m_pTarget->p_status->full_shape = bool_value;
		return;
	}
}
