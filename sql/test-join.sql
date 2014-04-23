---------------------- JOIN TEST CASES ----------------------

-- create relations
CREATE TABLE stars(starid integer, real_name char(20),
                   plays char(12), showid integer);

-- insert with attributes in order:
INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (100, 'Glover, Donald', 'Troy', 6);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (101, 'Brie, Alison', 'Annie', 6);

-- insert with out of order attributes
INSERT INTO stars (real_name, showid, starid, plays)
  VALUES ('McHale, Joel', 6, 102, 'Jeff');

-- insert with attributes in order:
INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (103, 'Jeong, Ken', 'Chang', 6);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (104, 'Radnor, Josh', 'Ted', 5);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (105, 'Hannigan, Alyson', 'Lily', 5);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (106, 'Segel, Jason', 'Marshall', 5);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (107, 'Vergara, Sofia', 'Gloria', 4);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (108, 'Bowen, Julie', 'Claire', 4);

INSERT INTO stars(starid, real_name, plays, showid)
  VALUES (109, 'Burrell, Ty', 'Phil', 4);

-- should give all stars
-- non equi-join, SNL
SELECT stars.real_name FROM stars WHERE stars.starid >= stars.showid;


CREATE TABLE show(showid integer, name char(32),
                   network char(4), rating double);

INSERT INTO show(showid, name, network, rating)
  VALUES (4, 'Modern Family', 'ABC', 4.1);

INSERT INTO show(showid, name, network, rating)
  VALUES (5, 'How I Met Your Mother', 'CBS', 4.5);

INSERT INTO show(showid, name, network, rating)
  VALUES (6, 'Community', 'NBC', 4.0);

-- should give all stars and show ID
-- equi-join, no index, should use SMJ
SELECT stars.real_name, show.name FROM stars, show WHERE stars.showid = show.showid;

CREATE INDEX stars (starid);

-- index NOT ON ATTR1 OR ATTR2 so it should STILL use SMJ
SELECT stars.real_name, show.name FROM stars, show WHERE stars.showid = show.showid;

CREATE INDEX stars (showid);

-- index on attr1, should use INL
SELECT stars.real_name, show.name FROM stars, show WHERE stars.showid = show.showid;

CREATE INDEX show (showid);

-- index on attr1 and attr2, should STILL use INL
SELECT stars.real_name, show.name FROM stars, show WHERE stars.showid = show.showid;

DROP INDEX stars (showid);

-- index on attr2, should STILL use INL
SELECT stars.real_name, show.name FROM stars, show WHERE stars.showid = show.showid;

DROP TABLE stars;
DROP TABLE show;
