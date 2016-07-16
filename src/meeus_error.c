#include "meeus_error.h"

char *meeus_perror(meeus_err err) {
	switch (err) {
	case MeeusError_Success:
		return "Success"; break;
	case MeeusError_Failure:
		return "Failure"; break;
	case MeeusError_NoMemory:
		return "Out of memory"; break;
	case MeeusError_InvalidInput:
		return "Invalid inputs"; break;
	case MeeusError_InvalidDate:
		return "Invalid date"; break;
	case MeeusError_Unknown:
	default:
		return "Unknown error condition"; break;
	}
}
