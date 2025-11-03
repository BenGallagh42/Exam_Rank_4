#include "argo.h"

// PART GIVEN IN SUBJECT
int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

// BEGINNING OF SOLUTION

static int parse_value(json *dst, FILE *stream);
static int parse_number(json *dst, FILE *stream);
static int parse_string(json *dst, FILE *stream);
static int parse_map(json *dst, FILE *stream);

// Main function
int	argo(json *dst, FILE *stream)
{
	int	ret;

	ret = parse_value(dst, stream);
	if (ret == 1 && peek(stream) != EOF)
	{
		unexpected(stream);
		free_json(*dst);
		return (-1);
	}
	return (ret);
}

// Find out what type to parse
static int	parse_value(json *dst, FILE *stream)
{
	int	c;

	c = peek(stream);
	if (c == '"')
		return (parse_string(dst, stream));
	if (c == '-' || isdigit(c))
		return (parse_number(dst, stream));
	if (c == '{')
		return (parse_map(dst, stream));
	unexpected(stream);
	return (-1);
}

static int	parse_number(json *dst, FILE *stream)
{
	int	sign;
	int	num;
	int	c;

	sign = 1;
	num = 0;
	if (accept(stream, '-'))
		sign = -1;
	if (!isdigit(peek(stream)))
	{
		unexpected(stream);
		return (-1);
	}
	while (isdigit(c = getc(stream)))
		num = num * 10 + (c - '0');
	if (c != EOF)
		ungetc(c, stream);
	dst->type = INTEGER;
	dst->integer = sign * num;
	return (1);
}

static int	parse_string(json *dst, FILE *stream)
{
	char	*str;
	size_t	len;
	size_t	cap;
	int		c;

	str = NULL;
	len = 0;
	cap = 0;
	if (!expect(stream, '"'))
		return (-1);
	while ((c = getc(stream)) != '"' && c != EOF)
	{
		if (c == '\\')
		{
			c = getc(stream);
			if (c != '\\' && c != '"' && c != EOF)
			{
				ungetc(c, stream);
				c = '\\';
			}
		}
		if (len + 2 > cap)
		{
			if (cap == 0)
				cap = 16;
			else
				cap = cap * 2;
			str = realloc(str, cap);
			if (!str)
				return (-1);
		}
		str[len++] = c;
	}
	if (c != '"')
	{
		free(str);
		unexpected(stream);
		return (-1);
	}
	if (len + 1 > cap)
	{
		cap = len + 1;
		str = realloc(str, cap);
		if (!str)
			return (-1);
	}
	str[len] = '\0';
	dst->type = STRING;
	dst->string = str;
	return (1);
}

static int	parse_map(json *dst, FILE *stream)
{
	pair	*pairs;
	size_t	size;
	size_t	cap;
	json	key_json;

	pairs = NULL;
	size = 0;
	cap = 0;
	if (!expect(stream, '{'))
		return (-1);
	if (accept(stream, '}'))
	{
		dst->type = MAP;
		dst->map.data = NULL;
		dst->map.size = 0;
		return (1);
	}
	while (1)
	{
		if (size >= cap)
		{
			if (cap == 0)
				cap = 4;
			else
				cap = cap * 2;
			pairs = realloc(pairs, cap * sizeof(pair));
			if (!pairs)
				goto error;
		}
		if (parse_string(&key_json, stream) != 1)
			goto error;
		pairs[size].key = key_json.string;
		if (!expect(stream, ':'))
			goto error;
		if (parse_value(&pairs[size].value, stream) != 1)
			goto error;
		size++;
		if (accept(stream, '}'))
			break ;
		if (!expect(stream, ','))
			goto error;
	}
	dst->type = MAP;
	dst->map.data = pairs;
	dst->map.size = size;
	return (1);

error:
	for (size_t i = 0; i < size; i++)
	{
		free(pairs[i].key);
		free_json(pairs[i].value);
	}
	free(pairs);
	return (-1);
}