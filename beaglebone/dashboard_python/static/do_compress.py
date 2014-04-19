#!/usr/bin/python
import os
import optparse
import re

YUI_COMPRESSOR = 'yuicompressor-2.4.7.jar'

SCRIPTS_MAIN = ['js/jquery_js/jquery.min.js',
                'js/jquery_js/jquery.ba-throttle-debounce.min.js',
                'js/bootstrap_js/bootstrap.js',
                'js/sidewinder_js/sidewinder.js']
SCRIPTS_MAIN_OUT = 'js/sidewinder.min.js'

LESS_TO_CSS = [('css/bootstrap_less/bootstrap.less',
                'css/bootstrap_less/bootstrap.css')]
STYLESHEETS_MAIN = ['css/bootstrap_less/bootstrap.css']
STYLESHEETS_MAIN_OUT = 'css/sidewinder.min.css'


def compress(in_files, out_file, in_type='js', verbose=False,
             temp_file='.temp'):
    temp = open(temp_file, 'w')
    for f in in_files:
        fh = open(f)
        data = fh.read() + '\n'
        fh.close()

        temp.write(data)

        print ' + %s' % f
    temp.close()

    options = ['-o "%s"' % out_file,
               '--type %s' % in_type]

    if verbose:
        options.append('-v')

    os.system('java -jar "%s" %s "%s"' % (YUI_COMPRESSOR,
                                          ' '.join(options),
                                          temp_file))

    org_size = os.path.getsize(temp_file)
    new_size = os.path.getsize(out_file)

    print '=> %s' % out_file
    print 'Original: %.2f kB' % (org_size / 1024.0)
    print 'Compressed: %.2f kB' % (new_size / 1024.0)
    print 'Reduction: %.1f%%' % (float(org_size - new_size) / org_size * 100)
    print ''

def compile_less(less_to_css):
    for less, css in less_to_css:
        os.system("lessc {} {}".format(less, css))

def main(kind=None):
    if kind == 'js' or kind == None:
        print 'Deleting Compiled JavaScript...'
        deleteFolderFiles('js')
        print 'Compressing JavaScript...'
        compress(SCRIPTS_MAIN, SCRIPTS_MAIN_OUT, 'js')

    if kind == 'css' or kind == None:
        print 'Deleting Compiled CSS...'
        deleteFolderFiles('css')
        print 'Compiling LESS...'
        compile_less(LESS_TO_CSS)
        print 'Compressing CSS...'
        compress(STYLESHEETS_MAIN, STYLESHEETS_MAIN_OUT, 'css')

def deleteFolderFiles(folder):
    for the_file in os.listdir(folder):
        file_path = os.path.join(folder, the_file)
        try:
            if os.path.isfile(file_path):
                os.unlink(file_path)
        except Exception, e:
            print e

if __name__ == '__main__':
    parser = optparse.OptionParser()
    options, args = parser.parse_args()
    if len(args) < 1:
        main()
    else:
        main(args[0])
