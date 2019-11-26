CREATE EXTENSION pg_simdjson;

-- Strings.
SELECT simdjson_parse('""');				-- OK.
SELECT simdjson_parse($$''$$);			-- ERROR, single quotes are not allowed
SELECT simdjson_parse('"abc"');			-- OK
SELECT simdjson_parse('"abc');			-- ERROR, quotes not closed
SELECT simdjson_parse('"abc
def"');				-- ERROR, unescaped newline in string constant
SELECT simdjson_parse('"\n\"\\"');		-- OK, legal escapes
SELECT simdjson_parse('"\v"');			-- ERROR, not a valid JSON escape
-- see json_encoding test for input with unicode escapes

-- Numbers.
SELECT simdjson_parse('1');				-- OK
SELECT simdjson_parse('0');				-- OK
SELECT simdjson_parse('01');				-- ERROR, not valid according to JSON spec
SELECT simdjson_parse('0.1');				-- OK
SELECT simdjson_parse('9223372036854775808');	-- OK, even though it's too large for int8
SELECT simdjson_parse('1e100');			-- OK
SELECT simdjson_parse('1.3e100');			-- OK
SELECT simdjson_parse('1f2');				-- ERROR
SELECT simdjson_parse('0.x1');			-- ERROR
SELECT simdjson_parse('1.3ex100');		-- ERROR

-- Arrays.
SELECT simdjson_parse('[]');				-- OK
SELECT simdjson_parse('[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]');  -- OK
SELECT simdjson_parse('[1,2]');			-- OK
SELECT simdjson_parse('[1,2,]');			-- ERROR, trailing comma
SELECT simdjson_parse('[1,2');			-- ERROR, no closing bracket
SELECT simdjson_parse('[1,[2]');			-- ERROR, no closing bracket

-- Objects.
SELECT simdjson_parse('{}');				-- OK
SELECT simdjson_parse('{"abc"}');			-- ERROR, no value
SELECT simdjson_parse('{"abc":1}');		-- OK
SELECT simdjson_parse('{1:"abc"}');		-- ERROR, keys must be strings
SELECT simdjson_parse('{"abc",1}');		-- ERROR, wrong separator
SELECT simdjson_parse('{"abc"=1}');		-- ERROR, totally wrong separator
SELECT simdjson_parse('{"abc"::1}');		-- ERROR, another wrong separator
SELECT simdjson_parse('{"abc":1,"def":2,"ghi":[3,4],"hij":{"klm":5,"nop":[6]}}'); -- OK
SELECT simdjson_parse('{"abc":1:2}');		-- ERROR, colon in wrong spot
SELECT simdjson_parse('{"abc":1,3}');		-- ERROR, no value

-- Recursion.
SET max_stack_depth = '100kB';
SELECT simdjson_parse(repeat('[', 10000));
SELECT simdjson_parse(repeat('{"a":', 10000));
RESET max_stack_depth;

-- Miscellaneous stuff.
SELECT simdjson_parse('true');			-- OK
SELECT simdjson_parse('false');			-- OK
SELECT simdjson_parse('null');			-- OK
SELECT simdjson_parse(' true ');			-- OK, even with extra whitespace
SELECT simdjson_parse('true false');		-- ERROR, too many values
SELECT simdjson_parse('true, false');		-- ERROR, too many values
SELECT simdjson_parse('truf');			-- ERROR, not a keyword
SELECT simdjson_parse('trues');			-- ERROR, not a keyword
SELECT simdjson_parse('');				-- ERROR, no value
SELECT simdjson_parse('    ');			-- ERROR, no value
