class Knapsack
{
	var name;
	var capacity;
	var items;
	var size;
	var max;
	var count;
	
	def Knapsack(self, name, capacity, items, size)
	{
		self.name = name;
		self.capacity = capacity;
		self.items = items;
		self.size = size;
		self.max = new Solution(self.name);
		self.count = 0;
	}
	
	def solve(self)
	{
		self.solve(0, new Solution(self.name));
		console::println(self.count);
		return self.max;
	}
	
	def solve(self, index, solution)
	{
		var with;
		var without;
		var items = self.items;
		var max = self.max;
		var item;
		var sum;
		self.count = self.count + 1;
		if(solution.cost > max.cost)
		{
			self.max = solution;
		} else{;}
		if(index >= self.size)
		{
			return;
		} else 
		{
			item = items[index];
			sum = solution.weight + item.weight;
			if(sum <= self.capacity)
			{
				with = solution.copy();
				with.add(item, index);
				self.solve(index + 1, with);
			} else {;}
			without = solution.copy();
			self.solve(index + 1, without);
		}
	}
	
	def main()
	{
		var filename = "input.txt";
		var in = new file();
		var id;
		var size;
		var capacity;
		var weight;
		var cost;
		var items[30];
		var i;
		var problem;
		var maximum;
		in.open(filename);
		
		id = in.readInt();
		size = in.readInt();
		if(size >= 30)
		{
			return 1;
		} else
		{
			capacity = in.readInt();
			for(i = 0; i < size; i = i + 1)
			{
				weight = in.readInt();
				cost = in.readInt();
				items[i] = new Item(weight, cost);
			}
			in.close();
			problem = new Knapsack(id, capacity, items, size);
			maximum = problem.solve();
			console::print(id);
			console::print(" ");
			console::println(maximum.cost);
		}
		
	}
}
