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
        
        jsmw[r'@entry'] = [{'tweet': r'@loop Hello MW!'}]
        jsmw[r'@loop'] = [{'query': {'from': r'@entry'}, 'tweet': r'@println $arg'}]

        return json.dumps(js)
