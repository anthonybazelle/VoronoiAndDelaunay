#include "Tools.h"

using namespace maths;




void Tools::triangulation()
{
	
	sommets.clear();
	cotes.clear();
	triangles.clear(); 


	
	
	std::sort(points.begin(), points.end(), points_comparator());


	
	
	int i = 0;

	if (points.size() == 0)
	{
		return;
	}

	if (points.size() == 1)
	{
		sommets.push_back(Sommet(&points.at(i)));
		return;
	}

	sommets.push_back(Sommet(&points.at(i)));
	sommets.push_back(Sommet(&points.at(i + 1)));
	cotes.push_back(Cote(&sommets.at(i), &sommets.at(i + 1)));
	sommets.at(i).a = &cotes.at(i);
	sommets.at(i + 1).a = &cotes.at(i);
	i += 2;

	
	while (i < points.size()
		&& abs(((points.at(i - 2).y - points.at(i - 1).y) * (points.at(i - 2).x - points.at(i).x))
			- ((points.at(i - 2).y - points.at(i).y) * (points.at(i - 2).x - points.at(i - 1).x)))
		<= colinearity_tolerance)
	{
		sommets.push_back(Sommet(&points.at(i)));
		cotes.push_back(Cote(&sommets.at(i - 1), &sommets.at(i)));
		sommets.at(i).a = &cotes.at(i - 1);
		++i;
	}

	if (i == points.size())
	{
		return;
	}

	int nedges = cotes.size();
	
	for (int j = 0; j < nedges; ++j)
	{
		sommets.push_back(Sommet(&points.at(i)));

		if (j == 0)
		{
			cotes.push_back(Cote(&sommets.at(i), cotes.at(j).s1));
			sommets.at(i).a = &cotes.at(i - 1);
		}

		cotes.push_back(Cote(cotes.at(j).s2, &sommets.at(i)));

		Triangle t = Triangle(&cotes.at(j), &cotes.at(cotes.size() - 2), &cotes.at(cotes.size() - 1));

		if (!isTriangleDirect(t))
		{
			t = Triangle(&cotes.at(j), &cotes.at(cotes.size() - 1), &cotes.at(cotes.size() - 2));
		}

		triangles.push_back(t);

		addTriangleToEdges(&triangles.back());

		if (j > 0)
		{
			cotes.at(cotes.size() - 3).tg = &triangles.at(j);
		}
	}

	
	
	float d, dref;
	Triangle* tref;
	Sommet a;
	Cote e1;
	Cote e2;
	Triangle tr;
	std::vector<Sommet*> tr_apexes;
	tr_apexes.reserve(2);
	std::vector<Cote*> seen_edges;
	seen_edges.reserve(maxPoints - 1);
	std::map<Sommet*, Cote*> added_edges;
	std::map<Sommet*, Cote*>::iterator it;
	Cote* tr_e1;
	Cote* tr_e2;

	for (int j = i + 1; j < points.size(); ++j)
	{
		
		sommets.push_back(Sommet(&points.at(j)));

		
		seen_edges = findSeenEdges(&sommets.back());

		//triangulate seen edges
		for (int k = 0; k < seen_edges.size(); ++k)
		{
			e1.s1 = &sommets.at(j);
			e1.s2 = seen_edges.at(k)->s1;
			e2.s1 = &sommets.at(j);
			e2.s2 = seen_edges.at(k)->s2;

			it = added_edges.find(seen_edges.at(k)->s1);
			//add new edge(s)
			if (it == added_edges.end())
			{
				cotes.push_back(e1);
				added_edges.insert(std::pair<Sommet*, Cote*>(seen_edges.at(k)->s1, &cotes.back()));
				tr_e1 = &cotes.back();
			}
			else
			{
				tr_e1 = it->second;
			}

			it = added_edges.find(seen_edges.at(k)->s2);
			if (it == added_edges.end())
			{
				cotes.push_back(e2);
				added_edges.insert(std::pair<Sommet*, Cote*>(seen_edges.at(k)->s2, &cotes.back()));
				tr_e2 = &cotes.back();
			}
			else
			{
				tr_e2 = it->second;
			}

			//create new triangle
			tr = Triangle(seen_edges.at(k), tr_e1, tr_e2);

			if (!isTriangleDirect(tr))
			{
				tr = Triangle(seen_edges.at(k), tr_e2, tr_e1);
			}

			//add new triangle to triangulation
			triangles.push_back(tr);

			//add triangle to edges
			addTriangleToEdges(&triangles.back());
		}

		//clear seen edges for next point
		seen_edges.clear();
		added_edges.clear();
	}
}

void Tools::delaunayTriangulation()
{
	std::vector<Cote*> edges_copy;
	edges_copy.reserve(maxPoints * (maxPoints - 1) / 2);

	for (int i = 0; i < cotes.size(); ++i)
	{
		edges_copy.push_back(&cotes.at(i));
	}

	Cote *a;
	Sommet *s1, *s2, *s3, *s4;
	Cote *a1, *a2, *a3, *a4;
	Triangle *t1, *t2;

	while (!edges_copy.empty())
	{
		a = edges_copy.back();
		edges_copy.pop_back();

		if (delaunayCriteria(a))
		{
			continue;
		}

		t1 = a->tg;
		t2 = a->td;

		if (t1->a1 == a)
		{
			a1 = t1->a2;
			a4 = t1->a3;
		}
		else if (t1->a2 == a)
		{
			a1 = t1->a3;
			a4 = t1->a1;
		}
		else
		{
			a1 = t1->a1;
			a4 = t1->a2;
		}

		if (t2->a1 == a)
		{
			a2 = t2->a3;
			a3 = t2->a2;
		}
		else if (t2->a2 == a)
		{
			a2 = t2->a1;
			a3 = t2->a3;
		}
		else
		{
			a2 = t2->a2;
			a3 = t2->a1;
		}

		s1 = a->s1;
		s2 = a->s2;

		if (a1->s1 == a->s1 || a1->s1 == a->s2)
		{
			s3 = a1->s2;
		}
		else
		{
			s3 = a1->s1;
		}

		if (a3->s1 == a->s1 || a3->s1 == a->s2)
		{
			s4 = a3->s2;
		}
		else
		{
			s4 = a3->s1;
		}

		a->s1 = s3;
		a->s2 = s4;
		s1->a = a1;
		s2->a = a4;

		t1->a1 = a;
		t1->a2 = a1;
		t1->a3 = a2;

		t2->a1 = a;
		t2->a2 = a3;
		t2->a3 = a4;

		if (!isTriangleDirect(*t1))
		{
			t1->a2 = a2;
			t1->a3 = a1;
		}
		if (!isTriangleDirect(*t2))
		{
			t2->a2 = a4;
			t2->a3 = a3;
		}

		for (Cote edge : cotes)
		{
			if (edge.td == t1 || edge.td == t2)
			{
				edge.td = nullptr;
			}
			if (edge.tg == t1 || edge.tg == t2)
			{
				edge.tg = nullptr;
			}
		}

		a->td = nullptr;
		a->tg = nullptr;

		addTriangleToEdges(t1);
		addTriangleToEdges(t2);

		if (std::find(edges_copy.begin(), edges_copy.end(), a1) == edges_copy.end())
		{
			edges_copy.push_back(a1);
		}
		if (std::find(edges_copy.begin(), edges_copy.end(), a2) == edges_copy.end())
		{
			edges_copy.push_back(a2);
		}
		if (std::find(edges_copy.begin(), edges_copy.end(), a3) == edges_copy.end())
		{
			edges_copy.push_back(a3);
		}
		if (std::find(edges_copy.begin(), edges_copy.end(), a4) == edges_copy.end())
		{
			edges_copy.push_back(a4);
		}
	}
}

void Tools::voronoiAlgo()
{
	if (points.size() < 3)
		return;

	regions.clear();

	Cercle c;
	Sommet *s1, *s2, *s3;
	Sommet *s4, *s5, *s6;
	std::map<Triangle*, Cercle> circonscript_circles;
	std::map<Triangle*, Cercle>::iterator it0;
	std::map<Triangle*, Cercle>::iterator it1;
	std::map<Cote*, Segment> bounded_edges;
	std::map<Cote*, Segment>::iterator it2;
	Cercle c1, c2;
	Point middle;

	for (int i = 0; i < triangles.size(); ++i)
	{
		std::tie(s1, s2, s3) = sommetsTriangle(&triangles.at(i));
		c = cercleCirconscrit(s1, s2, s3);

		circonscript_circles.insert(std::pair<Triangle*, Cercle>(&triangles.at(i), c));
	}

	for (int i = 0; i < cotes.size(); ++i)
	{
		if (cotes.at(i).td != nullptr && cotes.at(i).tg != nullptr)
		{
			it0 = circonscript_circles.find(cotes.at(i).td);
			c1 = it0->second;
			it1 = circonscript_circles.find(cotes.at(i).tg);
			c2 = it1->second;

			bounded_edges.insert(std::pair<Cote*, Segment>(&cotes.at(i), Segment(c1.center, c2.center)));
		}
		else
		{
			float dref, d;
			Sommet s;

			if (cotes.at(i).td == nullptr)
			{
				it1 = circonscript_circles.find(cotes.at(i).tg);
				c1 = it1->second;

				middle.x = (cotes.at(i).s1->p->x + cotes.at(i).s2->p->x) / 2;
				middle.y = (cotes.at(i).s1->p->y + cotes.at(i).s2->p->y) / 2;

				std::tie(s4, s5, s6) = sommetsTriangle(cotes.at(i).tg);
				if (s4 != cotes.at(i).s1 && s4 != cotes.at(i).s2)
				{
					dref = (s4->p->x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (s4->p->y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (c1.center.y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s5 != cotes.at(i).s1 && s5 != cotes.at(i).s2)
				{
					dref = (s5->p->x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (s5->p->y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (c1.center.y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s6 != cotes.at(i).s1 && s6 != cotes.at(i).s2)
				{
					dref = (s6->p->x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (s6->p->y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (c1.center.y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
			}
			else
			{
				it1 = circonscript_circles.find(cotes.at(i).td);
				c1 = it1->second;

				middle.x = (cotes.at(i).s1->p->x + cotes.at(i).s2->p->x) / 2;
				middle.y = (cotes.at(i).s1->p->y + cotes.at(i).s2->p->y) / 2;

				std::tie(s4, s5, s6) = sommetsTriangle(cotes.at(i).td);
				if (s4 != cotes.at(i).s1 && s4 != cotes.at(i).s2)
				{
					dref = (s4->p->x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (s4->p->y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (c1.center.y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s5 != cotes.at(i).s1 && s5 != cotes.at(i).s2)
				{
					dref = (s5->p->x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (s5->p->y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (c1.center.y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s6 != cotes.at(i).s1 && s6 != cotes.at(i).s2)
				{
					dref = (s6->p->x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (s6->p->y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - cotes.at(i).s1->p->x) * (cotes.at(i).s2->p->y - cotes.at(i).s1->p->y)
						- (c1.center.y - cotes.at(i).s1->p->y) * (cotes.at(i).s2->p->x - cotes.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
			}

			bounded_edges.insert(std::pair<Cote*, Segment>(&cotes.at(i), Segment(c1.center, middle)));
		}
	}

	for (int i = 0; i < sommets.size(); ++i)
	{
		std::vector<Segment> region;

		for (int j = 0; j < cotes.size(); ++j)
		{
			if (cotes.at(j).s1 == &sommets.at(i) || cotes.at(j).s2 == &sommets.at(i))
			{
				it2 = bounded_edges.find(&cotes.at(j));
				if (it2 == bounded_edges.end())
				{
					continue;
				}
				region.push_back(it2->second);
			}
		}

		regions.insert(std::pair<Sommet*, std::vector<Segment>>(&sommets.at(i), region));
		region.clear();
	}
}


bool Tools::isTriangleDirect(Triangle tr)
{
	Sommet *s1 = nullptr, *s2 = nullptr, *s3 = nullptr;

	std::tie(s1, s2, s3) = sommetsTriangle(&tr);

	return det(s1, s2, s3) < 0;
}

bool Tools::isTriangleLeft(Triangle tr, Cote* e)
{
	Sommet *s1 = nullptr, *s2 = nullptr, *s3 = nullptr;

	if (tr.a1 == e)
	{
		if (tr.a1->s1 == tr.a2->s1 || tr.a1->s2 == tr.a2->s1)
		{
			s1 = tr.a1->s1;
			s2 = tr.a1->s2;
			s3 = tr.a2->s2;
		}
		else if (tr.a1->s1 == tr.a2->s2 || tr.a1->s2 == tr.a2->s2)
		{
			s1 = tr.a1->s1;
			s2 = tr.a1->s2;
			s3 = tr.a2->s1;
		}
	}
	else if (tr.a2 == e)
	{
		if (tr.a2->s1 == tr.a3->s1 || tr.a2->s2 == tr.a3->s1)
		{
			s1 = tr.a2->s1;
			s2 = tr.a2->s2;
			s3 = tr.a3->s2;
		}
		else if (tr.a2->s1 == tr.a3->s2 || tr.a2->s2 == tr.a3->s2)
		{
			s1 = tr.a2->s1;
			s2 = tr.a2->s2;
			s3 = tr.a3->s1;
		}
	}
	else if (tr.a3 == e)
	{
		if (tr.a3->s1 == tr.a1->s1 || tr.a3->s2 == tr.a1->s1)
		{
			s1 = tr.a3->s1;
			s2 = tr.a3->s2;
			s3 = tr.a1->s2;
		}
		else if (tr.a3->s1 == tr.a1->s2 || tr.a3->s2 == tr.a1->s2)
		{
			s1 = tr.a3->s1;
			s2 = tr.a3->s2;
			s3 = tr.a1->s1;
		}
	}

	return det(s1, s2, s3) < 0;
}

float Tools::det(Sommet* s1, Sommet* s2, Sommet* s3)
{
	return 0.5 * ((s1->p->x - s2->p->x) * (s3->p->y - s2->p->y) - (s1->p->y - s2->p->y) * (s3->p->x - s2->p->x));
}

bool Tools::delaunayCriteria(Cote *a)
{
	if (a->td == nullptr || a->tg == nullptr)
	{
		return true;
	}

	Point c;
	float rc;
	Sommet *s1, *s2, *s3, *s4, *ts1, *ts2, *ts3;
	Cercle circonscript;

	s1 = a->s1;
	s2 = a->s2;

	std::tie(ts1, ts2, ts3) = sommetsTriangle(a->td);
	if (ts1 != s1 && ts1 != s2)
	{
		s3 = ts1;
	}
	else if (ts2 != s1 && ts2 != s2)
	{
		s3 = ts2;
	}
	else
	{
		s3 = ts3;
	}

	std::tie(ts1, ts2, ts3) = sommetsTriangle(a->tg);
	if (ts1 != s1 && ts1 != s2)
	{
		s4 = ts1;
	}
	else if (ts2 != s1 && ts2 != s2)
	{
		s4 = ts2;
	}
	else
	{
		s4 = ts3;
	}

	
	circonscript = cercleCirconscrit(s1, s2, s3);

	if (sqrt(pow(s4->p->x - circonscript.center.x, 2) + pow(s4->p->y - circonscript.center.y, 2)) <= circonscript.radius)
	{
		return false;
	}

	
	circonscript = cercleCirconscrit(s1, s2, s4);

	if (sqrt(pow(s3->p->x - circonscript.center.x, 2) + pow(s3->p->y - circonscript.center.y, 2)) <= circonscript.radius)
	{
		return false;
	}

	return true;
}

std::tuple<Sommet*, Sommet*, Sommet*> Tools::sommetsTriangle(Triangle* tr)
{
	Sommet *s1, *s2, *s3;

	if (tr->a1->s1 == tr->a2->s1)
	{
		s1 = tr->a1->s1;
		s2 = tr->a2->s2;
		s3 = tr->a1->s2;
	}
	else if (tr->a1->s1 == tr->a2->s2)
	{
		s1 = tr->a1->s1;
		s2 = tr->a2->s1;
		s3 = tr->a1->s2;
	}
	else if (tr->a1->s2 == tr->a2->s1)
	{
		s1 = tr->a1->s2;
		s2 = tr->a2->s2;
		s3 = tr->a1->s1;
	}
	else if (tr->a1->s2 == tr->a2->s2)
	{
		s1 = tr->a1->s2;
		s2 = tr->a2->s1;
		s3 = tr->a1->s1;
	}

	return std::make_tuple(s1, s2, s3);
}

Cercle Tools::cercleCirconscrit(Sommet* s1, Sommet* s2, Sommet* s3)
{
	Cercle c;

	float x1, x2, y1, y2;
	float a1, a2, b1, b2;

	x1 = (s1->p->x + s2->p->x) / 2;
	y1 = (s1->p->y + s2->p->y) / 2;
	x2 = (s1->p->x + s3->p->x) / 2;
	y2 = (s1->p->y + s3->p->y) / 2;

	a1 = (s1->p->y - s2->p->y) / (s1->p->x - s2->p->x);
	a2 = (s1->p->y - s3->p->y) / (s1->p->x - s3->p->x);

	b1 = y1 + 1 / a1 * x1;
	b2 = y2 + 1 / a2 * x2;

	c.center.x = (b2 - b1) / (1 / a2 - 1 / a1);
	c.center.y = -1 / a1 * c.center.x + b1;

	c.radius = sqrt(pow(s1->p->x - c.center.x, 2) + pow(s1->p->y - c.center.y, 2));

	return c;
}

bool Tools::pointIsInTriangle(Triangle* t, Sommet* s)
{
	float d, dref;
	Sommet *s1, *s2, *s3;
	std::tie(s1, s2, s3) = sommetsTriangle(t);

	if (t->a1->s1 != s1 && t->a1->s2 != s1)
	{
		dref = (s1->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
			- (s1->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else if (t->a1->s1 != s2 && t->a1->s2 != s2)
	{
		dref = (s2->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
			- (s2->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else
	{
		dref = (s3->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
			- (s3->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}

	d = (s->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
		- (s->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
	d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

	if (d != dref && d != 0)
	{
		return false;
	}

	if (d == 0)
	{
		return true;
	}

	if (t->a2->s1 != s1 && t->a2->s2 != s1)
	{
		dref = (s1->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
			- (s1->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else if (t->a2->s1 != s2 && t->a2->s2 != s2)
	{
		dref = (s2->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
			- (s2->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else
	{
		dref = (s3->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
			- (s3->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}

	d = (s->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
		- (s->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
	d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

	if (d != dref && d != 0)
	{
		return false;
	}

	if (d == 0)
	{
		return true;
	}

	if (t->a3->s1 != s1 && t->a3->s2 != s1)
	{
		dref = (s1->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
			- (s1->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else if (t->a3->s1 != s2 && t->a3->s2 != s2)
	{
		dref = (s2->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
			- (s2->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else
	{
		dref = (s3->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
			- (s3->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}

	d = (s->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
		- (s->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
	d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

	if (d != dref && d != 0)
	{
		return false;
	}

	return true;
}

std::vector<Cote*> Tools::findSeenEdges(Sommet* s)
{
	Triangle* tref;
	std::vector<Sommet*> tr_apexes;
	tr_apexes.reserve(2);
	Sommet a;
	std::vector<Cote*> seen_edges;
	seen_edges.reserve(cotes.size());
	float d, dref;

	for (int k = 0; k < cotes.size(); ++k)
	{
		//if both triangle non null => continue
		if (cotes.at(k).td != nullptr
			&& cotes.at(k).tg != nullptr)
		{
			continue;
		}

		//find non null triangle
		tref = (cotes.at(k).td != nullptr) ? cotes.at(k).td : cotes.at(k).tg;

		//add apexes of edge
		tr_apexes.push_back(cotes.at(k).s1);
		tr_apexes.push_back(cotes.at(k).s2);

		//find apex of triangle not in edge
		if (tref->a1 != &cotes.at(k))
		{
			if (std::find(tr_apexes.begin(), tr_apexes.end(), tref->a1->s1)
				!= tr_apexes.end())
			{
				a = *tref->a1->s2;
			}
			else
			{
				a = *tref->a1->s1;
			}
		}
		else if (tref->a2 != &cotes.at(k))
		{
			if (std::find(tr_apexes.begin(), tr_apexes.end(), tref->a2->s1)
				!= tr_apexes.end())
			{
				a = *tref->a2->s2;
			}
			else
			{
				a = *tref->a2->s1;
			}
		}

		//calculate reference value for side of edge
		dref = (a.p->x - cotes.at(k).s1->p->x) * (cotes.at(k).s2->p->y - cotes.at(k).s1->p->y)
			- (a.p->y - cotes.at(k).s1->p->y) * (cotes.at(k).s2->p->x - cotes.at(k).s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

		//calculate real value for side of edge
		d = (s->p->x - cotes.at(k).s1->p->x) * (cotes.at(k).s2->p->y - cotes.at(k).s1->p->y)
			- (s->p->y - cotes.at(k).s1->p->y) * (cotes.at(k).s2->p->x - cotes.at(k).s1->p->x);
		d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

		//check if edge is seen by point
		if (d != dref && d != 0)
		{
			seen_edges.push_back(&cotes.at(k));
		}

		tr_apexes.clear();
	}

	return seen_edges;
}

std::vector<Cote*> Tools::cotesAdjacents(Point* p)
{
	std::vector<Cote*> adjacent_edges;
	adjacent_edges.reserve(cotes.size());

	for (int i = 0; i < cotes.size(); ++i)
	{
		if (cotes.at(i).s1->p == p || cotes.at(i).s2->p == p)
		{
			adjacent_edges.push_back(&cotes.at(i));
		}
	}

	return adjacent_edges;
}

float Tools::orientedAngle(Cote* a1, Cote* a2, Point* p)
{
	Sommet *s = nullptr, *s1 = nullptr, *s2 = nullptr;

	for (int i = 0; i < sommets.size(); ++i)
	{
		if (sommets.at(i).p == p)
		{
			s = &sommets.at(i);
			continue;
		}
		if (sommets.at(i).p == a1->s1->p)
		{
			s1 = &sommets.at(i);
			continue;
		}
		if (sommets.at(i).p == a1->s2->p)
		{
			s1 = &sommets.at(i);
			continue;
		}
		if (sommets.at(i).p == a2->s1->p)
		{
			s2 = &sommets.at(i);
			continue;
		}
		if (sommets.at(i).p == a2->s2->p)
		{
			s2 = &sommets.at(i);
			continue;
		}
	}

	float angle = atan2(s2->p->y - s->p->y, s2->p->x - s->p->x) - atan2(s1->p->y - s->p->y, s1->p->x - s->p->x);
	if (angle < 0)
	{
		angle += 2 * PI;
	}

	return angle;
}

std::vector<Cote*> Tools::cotesOrienteAdjacents(Point* p)
{
	std::map<Cote*, float> oriented_angles;
	std::vector<Cote*> adjacent_edges = cotesAdjacents(p);
	adjacent_edges.reserve(cotes.size());
	Cote* aref = adjacent_edges.at(0);

	for (int i = 1; i < adjacent_edges.size(); ++i)
	{
		oriented_angles.insert(std::pair<Cote*, float>(adjacent_edges.at(i), orientedAngle(aref, adjacent_edges.at(i), p)));
	}

	typedef std::function<bool(std::pair<Cote*, float>, std::pair<Cote*, float>)> Comparator;

	Comparator compFunctor = [](std::pair<Cote*, float> elem1, std::pair<Cote*, float> elem2)
	{
		return elem1.second < elem2.second;
	};

	std::set<std::pair<Cote*, float>, Comparator> setOfAngles(oriented_angles.begin(), oriented_angles.end(), compFunctor);

	adjacent_edges.clear();
	adjacent_edges.push_back(aref);
	for (std::pair<Cote*, float> element : setOfAngles)
	{
		adjacent_edges.push_back(element.first);
	}

	return adjacent_edges;
}

std::vector<Triangle*> Tools::trianglesAdjacentsOrientes(std::vector<Cote*> adjacent_edges)
{
	std::vector<Triangle*> adjacent_triangles;
	adjacent_triangles.reserve(adjacent_edges.size());

	std::vector<Cote*>::iterator it1;
	std::vector<Cote*>::iterator it2;
	std::vector<Cote*>::iterator it3;

	for (int i = 0; i < adjacent_edges.size(); ++i)
	{
		for (int j = 0; j < triangles.size(); ++j)
		{
			it1 = std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(j).a1);
			it2 = std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(j).a2);
			it3 = std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(j).a3);

			if ((it1 == adjacent_edges.end() && it2 == adjacent_edges.end())
				|| (it1 == adjacent_edges.end() && it3 == adjacent_edges.end())
				|| (it2 == adjacent_edges.end() && it3 == adjacent_edges.end()))
			{
				continue;
			}

			if (std::find(adjacent_triangles.begin(), adjacent_triangles.end(), &triangles.at(j)) == adjacent_triangles.end())
			{
				adjacent_triangles.push_back(&triangles.at(j));
			}
			break;
		}
	}

	return adjacent_triangles;
}

std::vector<Cote*> Tools::cotesNonAdjacents(std::vector<Cote*> adjacent_edges, std::vector<Triangle*> adjacent_triangles)
{
	std::vector<Cote*> non_adjacent_edges;
	non_adjacent_edges.reserve(cotes.size());

	for (int i = 0; i < triangles.size(); ++i)
	{
		if (std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(i).a1) == adjacent_edges.end())
		{
			non_adjacent_edges.push_back(triangles.at(i).a1);
			continue;
		}

		if (std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(i).a2) == adjacent_edges.end())
		{
			non_adjacent_edges.push_back(triangles.at(i).a2);
			continue;
		}

		non_adjacent_edges.push_back(triangles.at(i).a3);
	}

	return non_adjacent_edges;
}

void Tools::addTriangleToEdges(Triangle* t)
{
	if (isTriangleLeft(*t, t->a1))
	{
		t->a1->tg = t;
	}
	else
	{
		t->a1->td = t;
	}

	if (isTriangleLeft(*t, t->a2))
	{
		t->a2->tg = t;
	}
	else
	{
		t->a2->td = t;
	}

	if (isTriangleLeft(*t, t->a3))
	{
		t->a3->tg = t;
	}
	else
	{
		t->a3->td = t;
	}
}

bool Tools::findEdge(Sommet* s1, Sommet* s2)
{
	for (Cote edge : cotes)
	{
		if ((edge.s1 == s1 && edge.s2 == s2)
			|| (edge.s1 == s2 && edge.s2 == s1))
		{
			return true;
		}
	}

	return false;
}

Tools::Tools()
{
	maxPoints = 50;
	colinearity_tolerance = 0.0001;
	 menuPrincipal = 0;

	 voronoi = false;
	 points.reserve(maxPoints);
	 sommets.reserve(maxPoints);
	 cotes.reserve(maxPoints * (maxPoints - 1) / 2);
	 triangles.reserve(maxPoints * maxPoints);
	 voronoiColor = { 1,0,0 };
}
