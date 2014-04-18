#!/bin/bash
make clean
make
./dbdestroy testdb
./dbcreate testdb
./minirel testdb sql/datamation.sql
./minirel testdb sql/select.sql
./minirel testdb sql/join.sql
