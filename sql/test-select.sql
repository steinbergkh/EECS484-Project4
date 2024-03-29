---------------------- SELECT TEST CASES ----------------------

DROP TABLE stars;

-- create relations
CREATE TABLE stars(starid integer, real_name char(20),
                   plays char(12), showid integer);

-- insert with attributes in order:
INSERT INTO stars(starid, real_name, plays, showid) VALUES (100, 'Glover, Donald', 'Troy', 6);

INSERT INTO stars(starid, real_name, plays, showid) VALUES (98, 'Brie, Alison', 'Annie', 6);

 -- insert with out of order attributes
 INSERT INTO stars(real_name, showid, starid, plays) VALUES ('McHale, Joel', 6, 99, 'Jeff');

-- select query hit
SELECT stars.starid, stars.showid, stars.real_name FROM stars WHERE stars.starid = 100;

-- unconditional select
SELECT stars.showid, stars.real_name FROM stars;

-- select query hit
SELECT * FROM stars WHERE stars.starid = 100;

-- select query miss
SELECT stars.starid, stars.showid, stars.real_name FROM stars WHERE stars.starid = 101;

-- select query table does not exist
SELECT * FROM starZ;

-- select query attribute does not exist
SELECT * FROM stars WHERE stars.starstatus = 100;

CREATE INDEX stars (starid);

-- select query with index on predicate, and test of equality, should call IndexSelect
SELECT stars.starid, stars.showid, stars.real_name FROM stars WHERE stars.starid = 100;

-- -- select query with index on predicate, but no equality test, should call HeapScan
SELECT stars.starid, stars.showid, stars.real_name FROM stars WHERE stars.starid < 100;

DROP TABLE stars;
