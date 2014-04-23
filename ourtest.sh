#!/bin/bash
make clean
make
./dbdestroy testdb
./dbcreate testdb
./minirel testdb sql/test-insert.sql
./minirel testdb sql/test-select.sql
./minirel testdb sql/test-join.sql
