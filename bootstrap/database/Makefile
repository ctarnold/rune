#  Copyright 2021 Google LLC.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

RUNE=../../rune

RUNE_TESTS=bigint block expr function statement value filepath location datatype

runtests: $(RUNE_TESTS)
	for test in $(RUNE_TESTS); do "./$$test"; done

expr: expr.rn
	$(RUNE) -g expr.rn

bigint: bigint.rn
	$(RUNE) -g bigint.rn

value: value.rn
	$(RUNE) -g value.rn

statement: statement.rn
	$(RUNE) -g statement.rn

block: block.rn
	$(RUNE) -g block.rn

function: function.rn
	$(RUNE) -g function.rn

filepath: filepath.rn
	$(RUNE) -g filepath.rn

location: location.rn
	$(RUNE) -g location.rn

datatype: datatype.rn
	$(RUNE) -g datatype.rn

clean:
	rm -f $(RUNE_TESTS) *.ll
