---------------------- INSERT TEST CASES ----------------------
DROP TABLE stars;
-- create relations
CREATE TABLE stars(starid integer, real_name char(20),
                   plays char(12), showid integer);

-- insert with attributes in order:
INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (100, 'Glover, Donald', 'Troy', 6);

-- insert with the same attribute twice:
INSERT INTO stars(starid, real_name, plays, plays, showid)
  VALUES (100, 'Glover, Donald', 'Troy', 'Troy', 6);

-- insert with duplicate attribute/data type mismatch :
INSERT INTO stars(starid, real_name, plays, plays)
  VALUES (100, 'Glover, Donald', 'Troy', 6);

-- insert with attributes out of order:
-- should still accept and insert correctly
INSERT INTO stars(real_name, showid, starid, plays)
  VALUES ('McHale, Joel', 3, 101, 'Jeff');

INSERT INTO stars(starid, real_name, plays, showid) VALUES (100, 'Ken Jeong', 'Chang', 6);

SELECT * FROM stars;

DROP TABLE stars;
