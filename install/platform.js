/**
 * Created by Aliaksei Syrel on 08/08/16.
 */

Config = require('./../config.js');
ConfigStatusParser = require('./configParser.js');
fs = require('fs');
_ = require('./libs/underscore.js');
Package = require('./package.js');

/*
 "static" singleton platform variable
 */
var platform;

ARCH_32 = 'i386';
ARCH_64 = 'x86_64';

function Platform() {
    var _this = this;

    /**
     * Return an array of package names to be compiled
     * @return {String[]}
     */
    _this.packages = function () {
        return _(_this.config().build.packages.general.concat(_this.platformPackages()))
            .map(function(name){
                return new Package({
                    name: name,
                    makefile: _this.makefile(),
                    sources: _this.sources(),
                    objects: _this.objects() });
            });
    };

    /**
     * Return an array of platform specific packages
     * @return {(Object[]|String[])}
     */
    _this.platformPackages = function () {
        return _this.array(_this.platform(_this.config().build.packages));
    };

    /**
     * Return a string containing %type% flags
     * that are common for all platforms
     */
    _this.generalFlags = function (type) {
        // flags cross target, general platform
        var flags = _this.string(_this.object((_this.object(_this.config().build.flags[type])).cross).general);
        // flags current target, general platform
        flags += ' ' + _this.string(_this.object(_this.target(_this.object(_this.config().build.flags[type]))).general);
        return flags;
    };

    /**
     * Return a platform specific %type% compiler flags,
     * that should be included in %type% Flags.
     */
    _this.platformFlags = function (type) {
        // flags cross target, current platform
        var flags = _this.string(_this.platform(_this.object((_this.object(_this.config().build.flags[type])).cross)));
        // flags current target, current platform
        flags += ' ' + _this.string(_this.platform(_this.object(_this.target(_this.object(_this.config().build.flags[type])))));
        return flags;
    };

    /**
     * Return a string containing all ASM compiler flags
     * @return {String}
     */
    _this.asmFlags = function () {
        var parser = new ConfigStatusParser();
        var flags = parser.parseASMFlags();
        flags += ' ' + _this.generalAsmFlags();
        flags += ' ' + _this.platformAsmFlags();
        return flags;
    };

    /**
     * Return a string containing ASM flags
     * that are common for all platforms
     */
    _this.generalAsmFlags = function () {
        return _this.generalFlags('asm');
    };

    /**
     * Return a platform specific ASM compiler flags,
     * that should be included in asmFlags.
     */
    _this.platformAsmFlags = function () {
        return _this.platformFlags('asm');
    };

    /**
     * Return a string containing all CXX compiler flags
     * @return {String}
     */
    _this.cxxFlags = function () {
        var parser = new ConfigStatusParser();
        var flags = parser.parseCXXFlags();
        flags += ' ' + parser.parseCompileCXXFlags();
        flags += ' ' + _this.generalCxxFlags();
        flags += ' ' + _this.platformCxxFlags();
        return flags;
    };

    /**
     * Return a string containing CXX flags
     * that are common for all platforms
     */
    _this.generalCxxFlags = function () {
        return _this.generalFlags('cxx');
    };

    /**
     * Return a platform specific CXX compiler flags,
     * that should be included in cxxFlags.
     */
    _this.platformCxxFlags = function () {
        return _this.platformFlags('cxx');
    };

    /**
     * Return a string containing all C compiler flags
     * @return {String}
     */
    _this.cFlags = function () {
        var parser = new ConfigStatusParser();
        var flags = parser.parseCFlags();
        flags += ' ' + parser.parseCompileCFlags();
        flags += ' ' + _this.generalCFlags();
        flags += ' ' + _this.platformCFlags();
        return flags;
    };

    /**
     * Return a string containing C flags
     * that are common for all platforms
     */
    _this.generalCFlags = function () {
        return _this.generalFlags('c');
    };

    /**
     * Return a platform specific C compiler flags,
     * that should be included in cFlags.
     */
    _this.platformCFlags = function () {
        return _this.platformFlags('c');
    };

    /**
     * Return a path to a C compiler
     * @return {String}
     */
    _this.cCompiler = function () {
        return new ConfigStatusParser().parseCCompiler();
    };

    /**
     * Return a string containing C compiler flags,
     * such as -std=gnu99
     * @return {String}
     */
    _this.cCompilerFlags = function () {
        return new ConfigStatusParser().parseCCompilerFlags();
    };

    /**
     * Return a path to a CXX compiler
     * @return {String}
     */
    _this.cxxCompiler = function () {
        return new ConfigStatusParser().parseCXXCompiler();
    };

    /**
     * Return a string containing CXX compiler flags,
     * such as -std=gnu99
     * @return {String}
     */
    _this.cxxCompilerFlags = function () {
        return new ConfigStatusParser().parseCXXCompilerFlags();
    };

    /**
     * Return a string containing all linker flags
     * @return {String}
     */
    _this.linkerFlags = function () {
        var flags = _this.generalLinkerFlags();
        flags += ' ' + _this.platformLinkerFlags();
        return flags;
    };

    _this.generalLinkerFlags = function () {
        return _this.string(_this.config().build.flags.linker.general);
    };

    _this.platformLinkerFlags = function () {
        return _this.string(_this.platform(_this.config().build.flags.linker));
    };

    _this.sourcesFlags = function () {
        return _this.array(_this.config().build.flags.sources);
    };

    _this.includes = function () {
        return _this.array(_this.config().build.includes);
    };

    _this.config = function () {
        return Config;
    };

    _this.configFlags = function () {
        return _this.config().config.flags;
    };

    _this.enabledModules = function () {
        return _this.array(_this.config().config.modules.enabled);
    };

    _this.disabledModules = function () {
        return _this.array(_this.config().config.modules.disabled);
    };

    _this.mozconfig = function () {
        return _this.config().config.mozconfig;
    };

    _this.mozconfigPath = function () {
        return _this.sources() + '/' + _this.mozconfig();
    };

    _this.makefile = function () {
        return _this.config().build.makefile;
    };

    _this.sources = function () {
        return _this.config().build.sources;
    };

    _this.objects = function () {
        return _this.sources() + '/' + _this.config().build.objects;
    };

    _this.project = function () {
        return _this.config().project;
    };

    _this.projectName = function () {
        return _this.project().name;
    };

    _this.cmakeVersion = function () {
        return _this.project().cmake;
    };

    _this.versionMajor = function () {
        return _this.project().version.major;
    };

    _this.versionMinor = function () {
        return _this.project().version.minor;
    };

    _this.configStatus = function () {
        return _this.config().config.status;
    };

    _this.crossCompile = function() {
        return _this.config().config.cross_compile;
    };

    _this.defines = function () {
        var defines = _this.string(_this.config().build.defines.general);
        return defines + ' ' + _this.platformDefines();
    };

    _this.platformDefines = function () {
        return _this.string(_this.platform(_this.config().build.defines));
    };

    _this.undefines = function () {
        var defines = _this.array(_this.config().build.undefines.general);
        return defines.concat(_this.platformUndefines());
    };

    _this.platformUndefines = function () {
        return _this.array(_this.platform(_this.config().build.undefines));
    };

    /**
     * Return an array of libraries that we need to link against
     * @return {Array.<Object>}
     */
    _this.libraries = function () {
        return _this.array(_this.platform(_this.config().build.libraries));
    };

    _this.bindingSources = function () {
        var sources = _this.array(_this.object(_this.config().build.bindings.general).sources);
        sources = sources.concat(_this.array(_this.object(_this.platform(_this.config().build.bindings)).sources));
        return sources;
    };

    _this.bindingPackages = function () {
        var packages = _this.array(_this.object(_this.config().build.bindings.general).packages);
        packages = packages.concat(_this.array(_this.object(_this.platform(_this.config().build.bindings)).packages));
        return packages;
    };

    _this.builder = function () {
        throw new Error('Subclass responsibility!');
    };

    _this.arch = function (arch) {
        if (_.isUndefined(arch))
            return _this.config().build.arch;
        _this.config().build.arch = arch;
        return _this;
    };

    /**
     * Transforms an undefined into an empty string
     * @param string
     * @return {String}
     */
    _this.string = function(string) {
        if (_.isUndefined(string)) return '';
        return string.toString();
    };

    /**
     * Transforms an undefined into an empty object
     * @param object
     * @return {Object}
     */
    _this.object = function(object) {
        if (_.isUndefined(object)) return {};
        return object;
    };

    /**
     * Transforms an undefined into an empty array
     * @param array
     * @return {Array.<Object>}
     */
    _this.array = function(array) {
        if (_.isUndefined(array)) return [];
        return array;
    };

    /**
     * Reuturn a target specific object field for my target,
     * or undefined if field does not exist.
     * @param anObject
     * @return {Object}
     */
    _this.target = function (anObject) {
        return _this.object(anObject[_this.arch()]);
    };

    /**
     * Return a platform specific object field for my platform,
     * or undefined if field does not exist.
     * @param anObject
     */
    _this.platform = function (anObject) {
        throw new Error('Subclass responsibility!');
    };

    _this.sourcesURL = function () {
        return _this.config().build.url + _this.config().build.format;
    };

    _this.sourcesArchive = function () {
        return _this.sources() + _this.config().build.format;
    };

    _this.patches = function () {
        return _(fs.readdirSync(_this.config().build.patches)).map(function(item) {
            return _this.config().build.patches + '/' + item;
        }).filter(function(item) {
            try { return fs.statSync(item).isFile(); }
            catch (e) { return false; }
        }).filter(function(file) {
            return file.endsWith('.patch');
        });
    };

    /**
     * Return true if we compile for 32 bits architecture,
     * false otherwise
     * @return {boolean}
     */
    _this.is32 = function () {
        return _this.arch() == ARCH_32;
    };

    /**
     * Return true if we compile for 64 bits architecture,
     * false otherwise
     * @return {boolean}
     */
    _this.is64 = function () {
        return _this.arch() == ARCH_64;
    };

    _this.output = function () {
        return _this.config().build.bin;
    };

    _this.log = function (object) {
        console.log(object);
    };

    /**
     * A visitor pattern
     * @param visitor
     * @return {*}
     */
    _this.accept = function (visitor) {
        return visitor.visitPlatform(_this);
    };

    _this.mozillaConfigH = function () {
        return _this.objects() + ' / ' + _this.config().build.mozilla_config;
    };
}

Platform.getPlatform = function () {
    if (!_.isUndefined(platform))
        return platform;

    var os = process.platform;
    if (os === 'darwin') {
        var PlatformMac = require('./platformMac.js');
        platform = new PlatformMac();
    }
    if (os === 'linux') {
        var PlatformLinux = require('./platformLinux.js');
        platform = new PlatformLinux();
    }

    if (_.isUndefined(platform))
        throw new Error('Unknown platform: ' + os);
    return platform;
};

module.exports = Platform;