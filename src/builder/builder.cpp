#include "builder/builder.h"
#include "util/tracer.h"
#include "builder/builder_context.h"

namespace builder {
builder builder::sentinel_builder;
var::operator builder () const {
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return builder::sentinel_builder;
	assert(block_var != nullptr);
	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);
		
	block::var_expr::Ptr var_expr = std::make_shared<block::var_expr>();
	var_expr->static_offset = offset;
	
	var_expr->var1 = block_var;
	builder_context::current_builder_context->add_node_to_sequence(var_expr);
	
	builder ret_block;
	ret_block.block_expr = var_expr;
	
	return ret_block;
}
void int_var::create_int_var(bool create_without_context) {
	
	if (create_without_context) {	
		block::var::Ptr int_var = std::make_shared<block::var>();	
		int_var->var_type = create_block_type();	
		block_var = int_var;
		return;
	}
	assert(builder_context::current_builder_context != nullptr);
	assert(builder_context::current_builder_context->current_block_stmt != nullptr);
	
	builder_context::current_builder_context->commit_uncommitted();
	
	block::var::Ptr int_var = std::make_shared<block::var>();	
	int_var->var_type = create_block_type();	
	block_var = int_var;
	
	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);

	int_var->static_offset = offset;
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return;
		
	block::decl_stmt::Ptr decl_stmt = std::make_shared<block::decl_stmt>();
	decl_stmt->static_offset = offset;
	
	decl_stmt->decl_var = int_var;
	decl_stmt->init_expr = nullptr;
	block_decl_stmt = decl_stmt;
	
	builder_context::current_builder_context->add_stmt_to_current_block(decl_stmt);
	
}	
int_var::int_var(bool create_without_context) {
	create_int_var(create_without_context);
}		
int_var::int_var(const int_var& a): int_var((builder)a) {
}
int_var::int_var(const builder& a) {
	builder_context::current_builder_context->remove_node_from_sequence(a.block_expr);
	create_int_var();
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return;
	block_decl_stmt->init_expr = a.block_expr;	
}
int_var::int_var(const int& a): int_var((builder)a) {
}


builder::builder (const int &a) {	
	assert(builder_context::current_builder_context != nullptr);
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return;
	block::int_const::Ptr int_const = std::make_shared<block::int_const>();
	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);
	//assert(offset != -1);
	int_const->static_offset = offset;
	int_const->value = a; 
	builder_context::current_builder_context->add_node_to_sequence(int_const);	
	
	block_expr = int_const;
	
}
template <typename T>
builder builder::builder_unary_op() {
	assert(builder_context::current_builder_context != nullptr);
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return builder::sentinel_builder;
	
	builder_context::current_builder_context->remove_node_from_sequence(block_expr);
	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);
	//assert(offset != -1);
	
	typename T::Ptr expr = std::make_shared<T>();
	expr->static_offset = offset;
	

	expr->expr1 = block_expr;

	builder_context::current_builder_context->add_node_to_sequence(expr);	
	
	builder ret_builder;
	ret_builder.block_expr = expr;
	return ret_builder;	
}
template <typename T>
builder builder::builder_binary_op(const builder &a) {
	assert(builder_context::current_builder_context != nullptr);
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return builder::sentinel_builder;
	
	builder_context::current_builder_context->remove_node_from_sequence(block_expr);
	builder_context::current_builder_context->remove_node_from_sequence(a.block_expr);

	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);
	//assert(offset != -1);
	
	typename T::Ptr expr = std::make_shared<T>();
	expr->static_offset = offset;
	

	expr->expr1 = block_expr;
	expr->expr2 = a.block_expr;

	builder_context::current_builder_context->add_node_to_sequence(expr);	
	
	builder ret_builder;
	ret_builder.block_expr = expr;
	return ret_builder;	
}

builder builder::operator && (const builder &a) {
	return builder_binary_op<block::and_expr>(a);
}
builder var::operator && (const builder &a) {
	return this->operator builder() && a;
}
builder operator && (const int &a, const builder &b) {
	return (builder)a && b;
}

builder builder::operator || (const builder &a) {
	return builder_binary_op<block::or_expr>(a);
}
builder var::operator || (const builder &a) {
	return this->operator builder() || a;
}
builder operator || (const int &a, const builder &b) {
	return (builder)a || b;
}

builder builder::operator + (const builder &a) {
	return builder_binary_op<block::plus_expr>(a);
}
builder var::operator + (const builder &a) {
	return this->operator builder() + a;
}
builder operator + (const int &a, const builder &b) {
	return (builder)a + b;
}

builder builder::operator - (const builder &a) {
	return builder_binary_op<block::minus_expr>(a);
}
builder var::operator - (const builder &a) {
	return this->operator builder() - a;
}
builder operator - (const int &a, const builder &b) {
	return (builder)a - b;
}

builder builder::operator * (const builder &a) {
	return builder_binary_op<block::mul_expr>(a);
}
builder var::operator * (const builder &a) {
	return this->operator builder() * a;
}
builder operator * (const int &a, const builder &b) {
	return (builder)a * b;
}

builder builder::operator / (const builder &a) {
	return builder_binary_op<block::div_expr>(a);
}
builder var::operator / (const builder &a) {
	return this->operator builder() / a;
}
builder operator / (const int &a, const builder &b) {
	return (builder)a / b;
}

builder builder::operator < (const builder &a) {
	return builder_binary_op<block::lt_expr>(a);
}
builder var::operator < (const builder &a) {
	return this->operator builder() < a;
}
builder operator < (const int &a, const builder &b) {
	return (builder)a < b;
}

builder builder::operator > (const builder &a) {
	return builder_binary_op<block::gt_expr>(a);
}
builder var::operator > (const builder &a) {
	return this->operator builder() > a;
}
builder operator > (const int &a, const builder &b) {
	return (builder)a > b;
}

builder builder::operator <= (const builder &a) {
	return builder_binary_op<block::lte_expr>(a);
}
builder var::operator <= (const builder &a) {
	return this->operator builder() <= a;
}
builder operator <= (const int &a, const builder &b) {
	return (builder)a <= b;
}

builder builder::operator >= (const builder &a) {
	return builder_binary_op<block::gte_expr>(a);
}
builder var::operator >= (const builder &a) {
	return this->operator builder() >= a;
}
builder operator >= (const int &a, const builder &b) {
	return (builder)a >= b;
}

builder builder::operator == (const builder &a) {
	return builder_binary_op<block::equals_expr>(a);
}
builder var::operator == (const builder &a) {
	return this->operator builder() == a;
}
builder operator == (const int &a, const builder &b) {
	return (builder)a == b;
}

builder builder::operator != (const builder &a) {
	return builder_binary_op<block::ne_expr>(a);
}
builder var::operator != (const builder &a) {
	return this->operator builder() != a;
}
builder operator != (const int &a, const builder &b) {
	return (builder)a != b;
}


builder builder::operator % (const builder &a) {
	return builder_binary_op<block::mod_expr>(a);
}
builder var::operator % (const builder &a) {
	return this->operator builder() % a;
}
builder operator % (const int &a, const builder &b) {
	return (builder)a % b;
}

builder builder::operator [] (const builder &a) {
	assert(builder_context::current_builder_context != nullptr);
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return builder::sentinel_builder;
	
	builder_context::current_builder_context->remove_node_from_sequence(block_expr);
	builder_context::current_builder_context->remove_node_from_sequence(a.block_expr);

	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);
	//assert(offset != -1);
	
	block::sq_bkt_expr::Ptr expr = std::make_shared<block::sq_bkt_expr>();
	expr->static_offset = offset;
	

	expr->var_expr = block_expr;
	expr->index = a.block_expr;

	builder_context::current_builder_context->add_node_to_sequence(expr);	
	
	builder ret_builder;
	ret_builder.block_expr = expr;
	return ret_builder;	
}
builder var::operator [] (const builder &a) {
	return this->operator builder () [a];
}

builder builder::operator ! () {
	return builder_unary_op<block::not_expr>();
}
builder var::operator! () {
	return !this->operator builder();
}
builder builder::operator = (const builder &a) {
	assert(builder_context::current_builder_context != nullptr);
	if (builder_context::current_builder_context->bool_vector.size() > 0)
		return builder::sentinel_builder;
	
	builder_context::current_builder_context->remove_node_from_sequence(block_expr);
	builder_context::current_builder_context->remove_node_from_sequence(a.block_expr);
	tracer::tag offset = get_offset_in_function(builder_context::current_builder_context->current_function);
	//assert(offset != -1);
	
	block::assign_expr::Ptr expr = std::make_shared<block::assign_expr>();
	expr->static_offset = offset;
	

	expr->var1 = block_expr;
	expr->expr1 = a.block_expr;

	builder_context::current_builder_context->add_node_to_sequence(expr);	
	
	builder ret_builder;
	ret_builder.block_expr = expr;
	return ret_builder;	
}
builder var::operator = (const builder &a) {
	return this->operator builder() = a;
}

builder::operator bool() {
	builder_context::current_builder_context->commit_uncommitted();
	return get_next_bool_from_context(builder_context::current_builder_context, block_expr);
}
var::operator bool() {
	return (bool)this->operator builder();
}
template <>
std::vector<block::type::Ptr> extract_type_vector<> (void) {
	std::vector<block::type::Ptr> empty_vector;
	return empty_vector;
}

template <>
std::vector <block::expr::Ptr> extract_call_arguments<> (void) {
	std::vector<block::expr::Ptr> empty_vector;
	return empty_vector;
}

}
