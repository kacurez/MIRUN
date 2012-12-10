class Item
{
	var weight;
	var cost;
	var ratio;

	def Item(self, weight, cost)
	{
		self.weight = weight;
		self.cost = cost;
		self.ratio = self.cost / self.weight;
	}
		
}
