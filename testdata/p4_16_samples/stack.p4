/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

header h {}

parser p()
{
    state start {
        h[12] stack;

        stack[3].setValid();
        h b = stack[3];
        b = stack.last;
        stack[3] = b;
        b = stack.next;
        stack.push_front(2);
        stack.pop_front(2);
    }
}

package top(p par);
top(p()) main;
