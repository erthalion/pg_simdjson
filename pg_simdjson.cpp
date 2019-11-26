#include "simdjson.h"
#include "simdjson.cpp"

#include "pg_simdjson.h"


void toJsonb(simdjson::ParsedJson::Iterator &pjh, JsonbInState *result,
			 JsonbIteratorToken tok)
{
	JsonbValue	v;

	if (pjh.is_object())
	{
		result->res = pushJsonbValue(&result->parseState,
									 WJB_BEGIN_OBJECT, NULL);

		if (pjh.down()) {
			/* Push key */
			v.type = jbvString;
			v.val.string.len = pjh.get_string_length();
			v.val.string.val = (char *) pjh.get_string();
			result->res = pushJsonbValue(&result->parseState, WJB_KEY, &v);

			pjh.next();
			toJsonb(pjh, result, WJB_VALUE);

			while (pjh.next())
			{
				v.type = jbvString;
				v.val.string.len = pjh.get_string_length();
				v.val.string.val = (char *) pjh.get_string();
				result->res = pushJsonbValue(&result->parseState, WJB_KEY, &v);

				pjh.next();
				toJsonb(pjh, result, WJB_VALUE);
			}
			pjh.up();
		}
		result->res = pushJsonbValue(&result->parseState,
									 WJB_END_OBJECT, NULL);
	}
	else if (pjh.is_array())
	{
		result->res = pushJsonbValue(&result->parseState,
									 WJB_BEGIN_ARRAY, NULL);
		if (pjh.down())
		{
			toJsonb(pjh, result, WJB_ELEM);

			while (pjh.next()) {
				toJsonb(pjh, result, WJB_ELEM);
			}

			pjh.up();
		}
		result->res = pushJsonbValue(&result->parseState,
									 WJB_END_ARRAY, NULL);
	}
	else
	{
		if (pjh.is_true())
		{
			v.type = jbvBool;
			v.val.boolean = true;
		}
		else if (pjh.is_false())
		{
			v.type = jbvBool;
			v.val.boolean = false;
		}
		else if (pjh.is_double())
		{
			Datum d = Float8GetDatum(pjh.get_double());

			v.type = jbvNumeric;
			v.val.numeric = DatumGetNumeric(DirectFunctionCall1(float8_numeric, d));
		}
		else if (pjh.is_integer())
		{
			Datum d = Int32GetDatum(pjh.get_integer());

			v.type = jbvNumeric;
			v.val.numeric = DatumGetNumeric(DirectFunctionCall1(int4_numeric, d));
		}
		else if (pjh.is_string())
		{
			v.type = jbvString;
			v.val.string.val = (char *) pjh.get_string();
			v.val.string.len = pjh.get_string_length();
		}
		else if (pjh.is_null())
		{
			v.type = jbvNull;
		}

		if (tok == WJB_DONE)
		{
			JsonbValue	va;

			va.type = jbvArray;
			va.val.array.rawScalar = true;
			va.val.array.nElems = 1;

			result->res = pushJsonbValue(&result->parseState, WJB_BEGIN_ARRAY, &va);
			result->res = pushJsonbValue(&result->parseState, WJB_ELEM, &v);
			result->res = pushJsonbValue(&result->parseState, WJB_END_ARRAY, NULL);
		}
		else
			result->res = pushJsonbValue(&result->parseState, tok, &v);
	}
}


Datum
pg_simdjson_parse(PG_FUNCTION_ARGS)
{
	text *json_as_text = PG_GETARG_TEXT_PP(0);

	simdjson::padded_string json(
		VARDATA_ANY(json_as_text),
		VARSIZE_ANY_EXHDR(json_as_text)
	);

	simdjson::ParsedJson pj = simdjson::build_parsed_json(json);

	if(!pj.is_valid())
	{
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_EXCEPTION),
				 errmsg("Parsed status: not valid, %s", Errors[pj.error_code])));
	}
	else
	{
		JsonbInState result;
		simdjson::ParsedJson::Iterator pjh(pj);
		if (!pjh.is_ok())
		{
			ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_EXCEPTION),
				 errmsg("Cannot create an iterator")));
		}

		memset(&result, 0, sizeof(JsonbInState));
		toJsonb(pjh, &result, WJB_DONE);

		PG_RETURN_POINTER(JsonbValueToJsonb(result.res));
	}
}
