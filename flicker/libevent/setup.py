
from distutils.core import setup, Extension
import os

LIBEVENT_ROOT = '/usr'
extra_link_args, libraries, library_dirs = [], [], []
include_dirs = [
    os.path.join(LIBEVENT_ROOT, 'include'),
    ]


libraries.append('rt')
libraries.append('pthread')

ext = Extension(
        'libevent',
        ['_libevent.c', '_base.c', '_event.c', '_buffer.c', '_bufferevent.c'],
        include_dirs=include_dirs,
        library_dirs=["/usr/local/lib"],
        libraries=['event'],
        # extra_link_args=extra_link_args,
        )

setup(
    name='libevent',
    version='1.0',
    description='libevent modules',
    ext_modules=[ext]
)
