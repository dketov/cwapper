# -*- encoding: utf-8 -*-

class Entity:
	class Property:
		def __init__(self, name, yaml):
			self.name = name
			self.yaml = yaml
			
	def __init__(self, name, yaml):
		self.name = name
		self.yaml = yaml
		self.properties = { 
			name: self.Property(name, yaml)
				for name, yaml in self.yaml.get('properties',{}).iteritems()
		}
		
	def __repr__(self):
		return "%s: %s" % (self.__class__.__name__, self.name)

	def __getattr__(self, name):
		return self.yaml.get(name)
