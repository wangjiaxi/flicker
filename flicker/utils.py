import logging
import re
from http import cookies

def parse_http_headers(headers):
    """
    :param headers: list[header]
    :return: dict(header)
    """
    re_header = re.compile(r'(.*): (.*)')
    result = []
    for header in headers:
        result.append(re.match(re_header, header).groups())
    return dict(result)


def parse_http_message(buffer):
    """parse http message
        :param buffer: http message
        :type  buffer: bytes or str
        :return: dict(
                    (str) method,
                    (str) path,
                    (str) protocol,
                    (dict) headers,
                    (SimpleCookie) _cookie,
                    (str) body)
        :rtype: dict
    """
    next_line = "\r\n"
    empty_line = "\r\n\r\n"
    if isinstance(buffer, bytes):
        buffer = buffer.decode('utf-8')
    try:
        request_line_and_header, request_data = buffer.split(empty_line)
        line_header_list = request_line_and_header.split(next_line)
        request_line = line_header_list.pop(0)
        method, path, protocol = request_line.split(" ")
        headers = parse_http_headers(line_header_list)

        try:
            cookie_s = headers.pop("Cookie")
            _cookie = cookies.SimpleCookie(cookie_s)
        except KeyError:
            _cookie = cookies.SimpleCookie()

    except ValueError as e:
        logging.error("Parse http message error: %s, buffer is not standard http message" % (e))
        return None

    return {
        "method": method,
        "path": path,
        "protocol": protocol,
        "_cookie": _cookie,
        "headers": headers,
        "body": request_data,
    }