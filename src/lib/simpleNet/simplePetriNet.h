#pragma once
#include <string>
#include <vector>
#include <algorithm>

class SimplePetriNet;
class SimpleNode
{
	protected:
		std::string _name;
		SimplePetriNet& _parent;

		std::vector<SimpleNode*> _preset;
		std::vector<SimpleNode*> _postset;

	public:
		SimpleNode(std::string name, SimplePetriNet& net):
			_name(name),
			_parent(net)
		{

		}

		virtual ~SimpleNode() = default;

		std::string getName() const
		{
			return _name;
		}

		void setName(const std::string& name)
		{
			_name = name;
		}

		std::vector<SimpleNode*>& getPreset()
		{
			return _preset;
		}

		std::vector<SimpleNode*>& getPostset()
		{
			return _postset;
		}

		void addNodeToPreset(SimpleNode* n)
		{
			_preset.push_back(n);
		}

		void addNodeToPostset(SimpleNode* n)
		{
			_postset.push_back(n);
		}

		void removeNodeFromPostset(SimpleNode* n)
		{
			auto it = std::find(_postset.begin(), _postset.end(), n);
			_postset.erase(it);
		}

		void removeNodeFromPreset(SimpleNode* n)
		{
			auto it = std::find(_preset.begin(), _preset.end(), n);
			_preset.erase(it);
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
		using SimpleNode::SimpleNode;
		int getCost() const
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
		SimpleNode* _orig;
		SimpleNode* _dest;

		SimplePetriNet& _parent;

	public:
		SimpleArc(SimpleNode* orig, SimpleNode* dest, SimplePetriNet& parent):
			_orig(orig),
			_dest(dest),
			_parent(parent)
		{
			orig->addNodeToPostset(dest);
			dest->addNodeToPreset(orig);
		}

		SimpleNode& getSourceNode()
		{
			return *_orig;
		}

		SimpleNode& getTargetNode()
		{
			return *_dest;
		}

		SimplePlace& getPlace();

		SimpleTransition& getTransition();

		void clear()
		{
			orig->removeNodeFromPostset(dest);
			dest->removeNodeFromPreset(orig);
		}
};

class SimplePetriNet
{
	private:
		std::vector<SimplePlace*> _places;
		std::vector<SimpleTransition*> _transitions;
		std::vector<SimpleArc*> _arcs;

	public:
		using node_type = SimpleNode;
		using transition_type = SimpleTransition;
		using arc_type = SimpleArc;
		using place_type = SimplePlace;
		SimplePlace& createPlace(std::string n)
		{
			_places.emplace_back(new SimplePlace(n, *this));
			return **(_places.end() - 1);
		}

		SimpleTransition& createTransition(std::string n)
		{
			_transitions.emplace_back(new SimpleTransition(n, *this));
			return **(_transitions.end() - 1);
		}

		SimpleArc& createArc(SimpleNode& orig, SimpleNode& dest)
		{
			_arcs.emplace_back(new SimpleArc(&orig, &dest, *this));
			return **(_arcs.end() - 1);
		}

		std::vector<SimplePlace*> getPlaces() const
		{
			return _places;
		}

		std::vector<SimpleTransition*> getTransitions() const
		{
			return _transitions;
		}

		std::vector<SimpleNode*> getNodes() const
		{
			std::vector<SimpleNode*> vec(_places.size() + _transitions.size());
			for(SimpleNode* n : _places) vec.push_back(n);
			for(SimpleNode* n : _transitions) vec.push_back(n);
			return vec;
		}

		const std::vector<SimpleArc*>& getArcs() const
		{
			return _arcs;
		}

		SimplePlace* findPlace(std::string s) const
		{
			for(SimplePlace* p : _places)
			{
				if(p->getName() == s) return p;
			}

			return nullptr;
		}
		SimpleTransition* findTransition(std::string s) const
		{
			for(SimpleTransition* t : _transitions)
			{
				if(t->getName() == s) return t;
			}

			return nullptr;
		}

		SimpleNode* findNode(std::string s) const
		{
			SimpleNode* ptr = findPlace(s);
			if(ptr) return ptr;

			ptr = findTransition(s);
			if(ptr) return ptr;

			return nullptr;
		}

		void deleteArc(SimpleArc& a)
		{
			a.getSourceNode().remov
		}
};



inline SimplePlace& SimpleArc::getPlace()
{
	auto place = std::find_if(std::begin(_parent.getPlaces()),
							  std::end(_parent.getPlaces()),
							  [&] (SimplePlace* p)
	{ return p->getName() == _orig->getName();});

	if(place != std::end(_parent.getPlaces()))
		return *dynamic_cast<SimplePlace*>(_orig);
	else
		return *dynamic_cast<SimplePlace*>(_dest);
}

inline SimpleTransition& SimpleArc::getTransition()
{
	auto trans = std::find_if(std::begin(_parent.getTransitions()),
							  std::end(_parent.getTransitions()),
							  [&] (SimpleTransition* p)
	{ return p->getName() == _orig->getName();});

	if(trans != std::end(_parent.getTransitions()))
		return *dynamic_cast<SimpleTransition*>(_orig);
	else
		return *dynamic_cast<SimpleTransition*>(_dest);
}
