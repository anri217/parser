# parser

The parser of the language whose chains are:

#Construction - branch operator
select <arithmetic expression>
case <const> <operators>
	[case <const> <operators>]
	[default <operators>]
end

<logical expression> → <comparison expression> | <unary logical operation><logical expression> | <logical expression><binary logical operation><logical expression>

<comparison expression> → <operand> | <operand><comparision operation><operand>

<comparison operation> → < | > | = | <>

<unary logical operation> → not

<binary logical operation> → and|or

<operand> → <identifier> | <const>

<operators> → <operators>;<operator> | <operator>

<operator> → <identifier> = <arithmetic expression>

<arithmetic expression> → <operand> | <operand><arithmetic operation><arithmetic expression>
<arithmetic operation> → + | - | / | *