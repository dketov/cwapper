# -*- encoding: utf-8 -*-
import yaml, json

class Route():
	def __init__(self, path, params, re, fid):
		self.path = path
		self.params = params
		self.re = re
		self.fid = fid

	@property
	def args(self):
		return ''.join(", %d" % i for i, _ in enumerate(self.params, 1))

	@property
	def signature(self):
		return ', '.join("std::string %s" % p for p in self.params)

class API:
	from path import Path
	from entity import Entity

	@property
	def description(self):
		return self.api.get('info', {}).get('description', '')

	@property
	def __json__(self):
		return json.dumps(self.api).replace('"', r'\"').replace(r'\n', r'\\n');
	@property
	def __yaml__(self):
		return yaml.dump(self.api).replace('"', r'\"').replace('\n', r'\n');
			
	def __init__(self, filename):
		with open(filename) as yamlfile:
			self.api = yaml.load(yamlfile)
		
		self.paths = tuple(
			sorted(
				(self.Path(self, uri, yaml)
					for uri, yaml in self.api.get('paths', {}).iteritems()),
				key = lambda path: path.uri # UGLY! curly braces '{' '}' are heavier than letters in ascii 
			)
		)
		
		self.definitions = {
			"#/definitions/%s" % name: self.Entity(name, yaml)
				for name, yaml in self.api.get('definitions', {}).iteritems()
		}

		self.routes = []
		
		for path in self.paths:
			uriRe = path.uri
			params = set()
			for parameter in path.parameters:
				if not parameter.iN('path'):
					continue
				uriParam = '{%s}' % parameter.name
				pattern = '([^/]+)' # param['type'] == 'string' or unkonwn
				if parameter.yaml.get('type') == 'integer':
					pattern = r'(\\d+)'
					
				if uriParam in path.uri:
					uriRe = uriRe.replace(uriParam, pattern)
					params.add(parameter.name)
			
			fid = path.uri.replace('/', '_').replace('{', '').replace('}', '')
			self.routes.append(Route(path, params, uriRe, fid))
