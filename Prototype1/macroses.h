#pragma once

// class helpers

#define DEFINE_GET(name) const decltype(m_##name)& get_##name() const { return m_##name; }
#define DEFINE_CONST(name) const char* name = #name

#define DECLARE_CONST_NAME(const_name) const std::wstring const_name = L#const_name

#define DECLARE_MIRR_CONST_NAME(const_name) {const_name, L#const_name}



