#include "argo.h"

// PART GIVEN IN SUBJECT — INCHANGÉ
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

// BEGINING OF SOLUTION
static int parse_value(json *dst, FILE *stream);
static int parse_number(json *dst, FILE *stream);
static int parse_string(json *dst, FILE *stream);
static int parse_map(json *dst, FILE *stream);

// FONCTION PRINCIPALE
int	argo(json *dst, FILE *stream)
{
	int	ret;

	ret = parse_value(dst, stream);
	if (ret == 1 && peek(stream) != EOF)
	{
		unexpected(stream);
		return (-1);
	}
	return (ret);
}

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
	int	sign = 1;
	int	num = 0;
	int	c;

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
	char	*str = NULL;
	size_t	len = 0;
	size_t	cap = 0;
	int		c;

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
		}
		str[len++] = c;
	}
	if (c != '"')
	{
		unexpected(stream);
		return (-1);
	}
	if (len + 1 > cap)
	{
		cap = len + 1;
		str = realloc(str, cap);
	}
	str[len] = '\0';
	dst->type = STRING;
	dst->string = str;
	return (1);
}

static int	parse_map(json *dst, FILE *stream)
{
	pair	*pairs = NULL;
	size_t	size = 0;
	size_t	cap = 0;
	json	key_json;

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
		}
		if (parse_string(&key_json, stream) != 1)
			return (-1);
		pairs[size].key = key_json.string;
		if (!expect(stream, ':'))
			return (-1);
		if (parse_value(&pairs[size].value, stream) != 1)
			return (-1);
		size++;
		if (accept(stream, '}'))
			break ;
		if (!expect(stream, ','))
			return (-1);
	}
	dst->type = MAP;
	dst->map.data = pairs;
	dst->map.size = size;
	return (1);
}