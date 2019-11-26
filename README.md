# pg_simdjson

A prototype for an extension, that generates jsonb via [simdjson](https://github.com/lemire/simdjson) library.

```
=# select simdjson_parse('{"a": {"b": 1, "c": 2, "d": "value"}}');
            simdjson_parse
---------------------------------------
 {"a": {"b": 1, "c": 2, "d": "value"}}
 (1 row)
```

All the tests from the original JSONB implementation are passing, but there is
an obvious difference in reported error messages. The only difference in
behaviour is parging of large numbers, e.g.

```
SELECT '9223372036854775808'::jsonb;
```

will be successfully parsed, while simdjson rejects that (see more in [Parser
Architechture and Implementation][1] section)

Here are results of parsing json with native PostgreSQL implementation and
simdjson. Test was performed in the benchmark environment on a pinned cpu
without hyperthreading and intel turbo with a dataset containing from 1 to 200
documents, each document is about 1.6 kB.

![benchmark](pg_simdjson.png?raw=true "Benchmark")

[1]: https://arxiv.org/pdf/1902.08318.pdf
