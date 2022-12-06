#pragma once
#include "Defines.hpp"
#include "Vector.hpp"
#include <xmmintrin.h>
#include <d3d9.h>
#include "globals.hpp"
#include <emmintrin.h>
#include "Kernel/Memory.h"

namespace Game
{
	// Updated to Thorn Update
	uint64_t GameManager()
	{
		return ((mem->RPM<uintptr_t>(globals.moduleBase + 0x71FBDD8) - 0x5013A972F916FB7Ei64) ^ 0x280DDB3232666F21i64) - 0x5A;
	}
	uintptr_t GetCamera()
	{
		uintptr_t cameraManager = mem->RPM<uintptr_t>(globals.moduleBase + 0x60F0398);
		cameraManager = mem->RPM<uintptr_t>(cameraManager + 0x40);
		cameraManager = mem->RPM<uintptr_t>(cameraManager + 0x0);
		return cameraManager;
	}

	static uint64_t decrypt_view_data(const uint64_t x)
	{
		return _rotl64(_rotl64(x, 25) - 6166909960704612984i64, 16);
	}

	static std::pair<uint64_t, uint32_t> get_entitylist(const uint64_t game_manager)
	{
		const auto decryption = [&game_manager](const uint32_t offset = 0) -> uint64_t
		{
			return mem->RPM<uint64_t>(game_manager + offset + 0xE8) ^ 0x87CA8BD06502E5FE;
		};

		return { decryption(), static_cast<uint32_t>(decryption(8) & 0x3FFFFFFF) };
	}
	static uint64_t Skeleton(const uint64_t pawn)
	{
		uint64_t rax = 0ull, rbx = 0ull, rcx = 0ull, rdx = 0ull, rdi = 0ull, rsi = 0ull, r8 = 0ull, r9 = 0ull, r10 = 0ull, r11 = 0ull, r12 = 0ull, r13 = 0ull, r14 = 0ull, r15 = 0ull, rbp = 0ull;

		r13 = pawn;
		rax = mem->RPM<uint64_t>(r13 + 0xA48);
		rax -= 0x78;

		return rax;
	}
	uint64_t Pawn(uint64_t address)
	{
		return (_rotl64(mem->RPM<uintptr_t>(address + 0x38), 40) ^ 0x52A261F3563389E0i64) - 108;
	}
	uint64_t component(uint64_t pawn)
	{
		return __ROL8__(mem->RPM<uintptr_t>(pawn + 0x18) + 0x3705D616EBB5E15Ei64, 1) + 0xA2A7BD6FF29CDA3i64;// not tested
	}
	float Health(uintptr_t pawn)
	{
		uintptr_t entity_actor = component(pawn);
		uintptr_t entity_info = mem->RPM<uintptr_t>(entity_actor + 0xD8);
		uintptr_t main_component = mem->RPM<uintptr_t>(entity_info + 0x8);
		return mem->RPM<int>(main_component + 0x1BC);
	}
	vec3 GetPos(uint64_t entPawn)
	{
		entPawn = mem->RPM<uint64_t>(entPawn + 0x18);
		return mem->RPM<vec3>(entPawn + 0x50);
	}
	void TransformsCalculation(__int64 pBones, __m128* ResultPosition, __m128* BoneInfo)
	{
		__m128 v5; // xmm2
		__m128 v6; // xmm3
		__m128 v7; // xmm0
		__m128 v8; // xmm4
		__m128 v9; // xmm1

		__m128 v10 = { 0.500f, 0.500f, 0.500f, 0.500f };
		__m128 v11 = { 2.000f, 2.000f, 2.000f, 0.000f };

		__m128 v12 = mem->RPM<__m128>(pBones);
		__m128 v13 = mem->RPM<__m128>(pBones + 0x10);

		v5 = v13;

		v6 = _mm_mul_ps(*(__m128*)BoneInfo, v5);
		v6.m128_f32[0] = v6.m128_f32[0]
			+ (float)(_mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0x55)) + _mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0xAA)));
		v7 = _mm_shuffle_ps(v13, v5, 0xFF);
		v8 = _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(v5, v5, 0xD2), *(__m128*)BoneInfo),
			_mm_mul_ps(_mm_shuffle_ps(*(__m128*)BoneInfo, *(__m128*)BoneInfo, 0xD2), v5));
		v9 = _mm_shuffle_ps(v12, v12, 0x93);
		v9.m128_f32[0] = 0.0;
		*(__m128*)ResultPosition = _mm_add_ps(
			_mm_shuffle_ps(v9, v9, 0x39),
			_mm_mul_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(_mm_shuffle_ps(v8, v8, 0xD2), v7),
						_mm_mul_ps(_mm_shuffle_ps(v6, v6, 0), v5)),
					_mm_mul_ps(
						_mm_sub_ps(_mm_mul_ps(v7, v7), (__m128)v10),
						*(__m128*)BoneInfo)),
				(__m128)v11));
	}
	vec3 GetBoneLocationByIndex(uintptr_t player, int index)
	{
		__m128 arrResultPosition;
		uintptr_t pBonesChain1 = Game::Skeleton(player);
		uintptr_t pBonesChain2 = mem->RPM<uintptr_t>(pBonesChain1);
		uintptr_t pBones = mem->RPM<uintptr_t>(pBonesChain2 + 0x250);
		uintptr_t pBonesData = mem->RPM<uintptr_t>(pBones + 0x58);
		uintptr_t idarray = mem->RPM<std::uintptr_t>(pBonesChain2 + 0xB8);
		index = mem->RPM<int>(idarray + index * 0x4);
		__m128 pBoneInfo = mem->RPM<__m128>((0x20 * index) + pBonesData);
		TransformsCalculation(pBones, &arrResultPosition, &pBoneInfo);
		return { arrResultPosition.m128_f32[0], arrResultPosition.m128_f32[1], arrResultPosition.m128_f32[2] };
	}
	auto decrypt_camera(__m128i encrypted) -> vec3
	{
		__m128i vector = _mm_load_si128(&encrypted);
		vector.m128i_u64[0] = _rotl64(_rotl64(vector.m128i_u64[0], 0x19) - 0x559543EF819DDA78, 0x10);
		vector.m128i_u64[1] = _rotl64(_rotl64(vector.m128i_u64[1], 0x19) - 0x559543EF819DDA78, 0x10);
		return *reinterpret_cast<vec3*>(&vector);
	}
	static bool WorldToScreen(vec3 world, vec2& output)
	{
		vec3 temp = world - decrypt_camera(mem->RPM<__m128i>(globals.cam + 0x130 + 0x40)); // translation
		float x = temp.Dot(decrypt_camera(mem->RPM<__m128i>(globals.cam + 0x130))); //right
		float y = temp.Dot(decrypt_camera(mem->RPM<__m128i>(globals.cam + 0x130 + 0x20))); // up
		float z = temp.Dot(decrypt_camera(mem->RPM<__m128i>(globals.cam + 0x130 + 0x30)) * -1.f); // forward

		int width = globals.width;
		int height = globals.height;

		output.x = (width / 2.f) * (1.f + x / - mem->RPM<float>(globals.cam + 0x360) / z); // fovx
		output.y = (height / 2.f) * (1.f - y / - mem->RPM<float>(globals.cam + 0x364) / z); // fovy

		if (output.x < 0.0f || output.x > width)
			return false;
		return z >= 1.0f;
	}
	uint64_t ReplicationInfo(uint64_t controller)
	{
		return _rotl64(_rotl64(mem->RPM<uint64_t>(controller + 0x70), 38) - 94, 32);
	}
	std::string name(uintptr_t entity)
	{
		uintptr_t replicationInfo = Game::ReplicationInfo(entity);
		return mem->RPM<std::string>(mem->RPM<uintptr_t>(replicationInfo + 0x9C0));
	}
	static uint32_t get_team_id(const uint64_t replicationinfo)
	{
		auto v13 = mem->RPM<uintptr_t>(replicationinfo + 0x728) - 88;

		if ((v13 ^ 0x8087EB2FEBB40640ui64) == 23)
			return 6;

		auto v7 = ((((v13 ^ 0x8087EB2FEBB40640ui64) - 23 + 0xB4) + 0x4DE0D74E) ^ 0x11A281F9) - 27;
		return mem->RPM<uintptr_t>(v7);
	}
	const char* OperatorNames[29][6] =
	{
		{"AI","SMOKE","MUTE","SLEDGE","THATCHER",""}, //0
		{"RECRUIT","CASTLE","ASH","PULSE","THERMITE",""}, //1
		{"","DOC","ROOK","TWITCH","MONTAGNE",""}, //2
		{"","GLAZ","FUZE","KAPKAN","TACHANKA",""}, //3
		{"","BLITZ","IQ","JAGER","BANDIT",""}, //4
		{"","BUCK","FROST","","",""}, //5
		{"","BLACKBEARD","VALKYRIE","","",""}, //6
		{"","CAPITAO","CAVEIRA","","",""}, //7
		{"","HIBANA","ECHO","","",""}, //8
		{"","JACKAL","MIRA","","",""}, //9
		{"","YING","LESION","","",""}, //10
		{"","ELA","ZOFIA","","",""}, //11
		{"","DOKKAEBI","VIGIL","","",""}, //12
		{"","","LION","FINKA","",""}, //13
		{"","MAESTRO","ALIBI","","",""}, //14
		{"","MAVERICK","CLASH","","",""}, //15
		{"","NOMAD","KAID", "","",""}, //16
		{"","MOZZIE","GRIDLOCK","","",""}, //17
		{"","NOKK","","","",""}, //18
		{"","WARDEN","","","",""}, //19
		{"","GOYO","","","",""}, //20
		{"","AMARU","","","",""}, //21
		{"","KALI","WAMAI", "ACE", "ARUNI", "OSA"}, //22
		{"","ORYX", "FLORES","","",""}, //23
		{"","IANA","","","",""}, //24
		{"","MELUSI","","","",""}, //25
		{"ZERO","","","","",""}, //26
		{"THUNDERBIRD","","","","",""}, //26
		{"THORN","","","","",""} // 27
	};
	auto OpName(uintptr_t player) -> const char*
	{
		uintptr_t replicationInfo = Game::ReplicationInfo(player);
		auto ctu = mem->RPM<uint8_t>(replicationInfo + 0x118);
		auto op = mem->RPM<uint8_t>(replicationInfo + 0x119);
		return OperatorNames[ctu % 29][op % 6];
	}
};



