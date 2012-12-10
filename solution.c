class Solution
{
	var id;
	var items;
	var cost;
	var weight;
	
	def Solution(self, name)
	{
		var i;
		var item;
		var items[30];
		self.id = name;
		self.items = items;
		self.cost = 0;
		self.weight = 0;
	}
			
	def add(self, item, index)
	{
		var tmp = self.items;
		tmp[index] = item;
		if(object::isNull(item) == 0)
		{
			self.cost = self.cost + item.cost;
			self.weight = self.weight + item.weight;
		} else{;}
	}
	
	def copy(self)
	{
		var copy = new Solution(self.id);
		var i;
		var tmp = self.items;
		for(i = 0; i < tmp.size(); i = i + 1)
		{
			copy.add(tmp[i], i);
		}
		return copy;
	}
}
