# -*- coding: utf-8 -*-
import json

class Generator(object):
    def __init__(self):
        pass
    
    def gen(self):
        js = {}
        js['Who creates MW?'] = 'Machine'
        js['MW Creator name'] = 'class Generator'

        jsmw = js['MW CU'] = {}
        
        jsmw['@entry'] = [{'tweet', '@loop Hello MW!'}]
        jsmw['@loop'] = [{'query': {'from': '@entry'}, 'tweet': '@println $arg'}]

        return json.dump(js)
