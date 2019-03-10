# -*- coding: utf-8 -*-
import json
from pprint import pprint

class ExpressionGenerator(object):
    def __init__(self):
        pass
    
    def codegen(self):
        js = {}
        js['Who creates MW?'] = 'Machine'
        js['MW Creator name'] = 'class ExpressionGenerator'

        jsmw = js['MW CU'] = {}
        
        jsmw[r'@entry'] = [{'ast': 'assign',
                            'LHS': {'ast': 'lvalue',
                                    'type number': -64.5,
                                    'name': 'v'},
                            'RHS': {'ast': 'plus',
                                    'type number': -64.5,
                                    'LHS': {'ast': 'fvalue',
                                            'type number': -64.5,
                                            'value': 1.0},
                                    'RHS': {'ast': 'fvalue',
                                            'type number': -64.5,
                                            'value': 2.0}}}]

        print('code json:')
        pprint(js)

        return json.dumps(js)

    def codegen_jsonnet(self):
        js = {}
        js['Who creates MW?'] = 'Machine'
        js['MW Creator name'] = 'class ExpressionGenerator'

        jsmw = js['MW CU'] = {}
        
        jsmw[r'@entry'] = [{'ast': 'assign',
                            'LHS': {'ast': 'lvalue',
                                    'type number': -64.5,
                                    'name': 'v'},
                            'RHS': {'ast': 'plus',
                                    'type number': -64.5,
                                    'LHS': {'ast': 'fvalue',
                                            'type number': -64.5,
                                            'value': 1.0},
                                    'RHS': {'ast': 'fvalue',
                                            'type number': -64.5,
                                            'value': 2.0}}}]

        print('code json:')
        pprint(js)

        return json.dumps(js)
