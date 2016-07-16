#ifndef MEEUS_ERROR_H
#define MEEUS_ERROR_H

/**
 * Error codes returned by functions in meeus package
 */
enum MeeusError {
	MeeusError_Unknown = -2, /*< unknown condition      */
	MeeusError_Failure = -1, /*< non-specific failure   */
	MeeusError_Success = 0,	 /*< non-specific success   */
	MeeusError_NoMemory, 	 /*< insufficient memory    */
	MeeusError_InvalidInput, /*< invalid inputs/args    */
	MeeusError_InvalidDate	 /*< given date is invalid  */
};
typedef enum MeeusError meeus_err;

/**
 * Return string describing error code `err`
 */
char *meeus_perror(meeus_err err);

#endif /* MEEUS_ERROR_H */
