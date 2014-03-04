#pragme once
#include <string>
#include <vector>
#include <algorithm>

class SimpleNode
{
	protected:
		std::string _name;

	public:
		SimpleNode(std::string name): _name(name)
		{

		}

		~SimpleNode() = default;

		std::string name() const
		{
			return _name;
		}
		void setName(const std::string& name)
		{
			_name = name;
		}
};

class SimplePlace : public SimpleNode
{
	private:
		int _tokens;
	public:
		using SimpleNode::SimpleNode;

		int getTokenCount() const
		{
			return _tokens;
		}
		void setTokenCount(int tokens)
		{
			_tokens = tokens;
		}
};

class SimpleTransition : public SimpleNode
{
	private:
		int _cost;

	public:
		SimpleTransition(std::string name, int cost):
			SimpleNode(name),
			_cost(cost)
		{
		}

		int cost() const
		{
			return _cost;
		}
		void setCost(int cost)
		{
			_cost = cost;
		}
};

class SimpleArc
{
	private:
		SimplePlace& _place;
		SimpleTransition& _trans;

	public:
		SimpleArc(SimplePlace& place, SimpleTransition& trans):
			_place(place),
			_trans(trans)
		{
		}
};

class SimplePetriNet
{

	private:
		std::vector<SimplePlace> _places;
		std::vector<SimpleTransition> _transitions;
		std::vector<SimpleArcs> _arcs;

	public:
		void addPlace(std::string n)
		{
			_places.emplace_back(n);
		}

		void addTrans(std::string n, int c)
		{
			_transitions.emplace_back(n, c);
		}

		void addArc(std::string orig, std::string dest)
		{
			bool placePosition = false; // False pour origin, true pour destination
			auto place = std::find_if(std::begin(_places), std::end(_places), [orig] (SimplePlace& p)
				{ return p.name() == orig;});
			if(place == std::end(_places))
			{
				place = std::find_if(std::begin(_places), std::end(_places), [dest] (SimplePlace& p)
								{ return p.name() == dest;});
				placePosition = true;
				if(place == std::end(_places)) throw "Bad place";
			}

			bool transPosition = false; // False pour origin, true pour destination
			auto trans = std::find_if(std::begin(_transitions), std::end(_transitions), [orig] (SimpleTransition& p)
				{ return p.name() == orig;});
			if(trans == std::end(_transitions))
			{
				trans = std::find_if(std::begin(_transitions), std::end(_transitions), [dest] (SimpleTransition& p)
								{ return p.name() == dest;});
				transPosition = true;
				if(trans == std::end(_transitions)) throw "Bad transition";
			}

			if(!placePosition && transPosition)
				_arcs.emplace_back(*place, *trans);
			else if(placePosition && !transPosition)
				_arcs.emplace_back(*trans, *place);
			else throw "Invalid request";
		}
};

