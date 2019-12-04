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
an obvious difference in reported error messages.

Here are results of parsing json with native PostgreSQL implementation and
simdjson. Test was performed in the benchmark environment on a pinned cpu
without hyperthreading and intel turbo with a dataset containing from 1 to 200
documents, each document is about 1.6 kB. Plotted graph here represents parsing
query latency, i.e. the whole processing time that includes not only json
parsing, but also everything to actually perform a query.

![benchmark](pg_simdjson.png?raw=true "Benchmark")

The idea behind this is to see how this extension behaves in a relatively real
situation. Actual json parsing part with simdjson takes just a small fraction
of time, e.g. for 200 documents it's between 256 – 511 microseconds. This
corresponds to approximately 1.3 GB/s and somehow close to the numbers,
reported in the simdjson repository.

[1]: https://arxiv.org/pdf/1902.08318.pdf
