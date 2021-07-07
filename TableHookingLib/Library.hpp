#pragma once
#include <memory>
#include <array>
#include <atomic>
#include <Windows.h>

namespace
{
#ifdef _M_IX86
	static auto jmp = std::to_array<std::uint8_t>({ 0xB8, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xE0 });
	constexpr auto base = 1;
#else
	static auto jmp = std::to_array<std::uint8_t>({ 0x48, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xE0 });
	constexpr auto base = 2;
#endif
}

class t_hook
{
	std::array<std::uint8_t, jmp.size()> original;
	std::atomic<bool> enabled;

	void* function = nullptr;
	void* detour = nullptr;

	void write(void* dst, const void* src, std::size_t size)
	{
		DWORD original_page;
		if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &original_page))
			throw std::exception("failed to unprotect");

		std::memmove(dst, src, size);

		VirtualProtect(dst, size, original_page, &original_page);
	}

public:
	void enable()
	{
		if (enabled.load())
			return;

		write(function, jmp.data(), jmp.size());

		enabled.exchange(true);
	}

	void disable()
	{
		if (!enabled.load())
			return;

		write(function, original.data(), original.size());
		enabled.exchange(false);
	}

	bool is_enabled() const
	{
		return enabled;
	}

	t_hook(void* function, void* detour) : function(function), detour(detour)
	{
		std::memmove(jmp.data() + base, &detour, sizeof(detour));
		std::memmove(original.data(), function, original.size());
	}

	~t_hook() 
	{
		disable();
	}
};