extern "C" {
	#include <postgres.h>
	#include <fmgr.h>
	#include <utils/jsonb.h>
	#include "utils/numeric.h"
	#include "utils/fmgrprotos.h"

	#ifdef PG_MODULE_MAGIC
		PG_MODULE_MAGIC;
	#endif

	PG_FUNCTION_INFO_V1(pg_simdjson_parse);
	Datum pg_simdjson_parse(PG_FUNCTION_ARGS);

	typedef struct JsonbInState
	{
		JsonbParseState *parseState;
		JsonbValue *res;
	} JsonbInState;

	Jsonb *JsonbValueToJsonb(JsonbValue *val);

	const char *Errors[] = {
		"No errors",
		"No errors and buffer still has more data",
		"This ParsedJson can't support a document that big",
		"Error allocating memory, we're most likely out of memory",
		"Something went wrong while writing to the tape",
		"Document exceeds the user-specified depth limitation",
		"Problem while parsing a string",
		"Problem while parsing an atom starting with the letter 't'",
		"Problem while parsing an atom starting with the letter 'f'",
		"Problem while parsing an atom starting with the letter 'n'",
		"Problem while parsing a number",
		"The input is not valid UTF-8",
		"Unitialized",
		"Empty",
		"Within strings, some characters must be escapted, we "
							"found unescapted characters",
		"Missing quote at the end",
		"Unexpected error, consider reporting this problem as "
							 "you may have found a bug in simdjson",
	};
}
