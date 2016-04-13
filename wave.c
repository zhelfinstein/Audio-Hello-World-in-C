#include "wave.h"
#define MAX_AMP 8192

int main(int argc, char** argv) {
	if(argc < 3) {
		printf("Usage: ./wave [r|w] filename\n");
		exit(1);
	}	
	char mode = argv[1][0];
	if(mode != 'r' && mode != 't' && mode != 'w') {
		printf("Usage: ./wave [r|w] filename\n");
		exit(4);
	}

	if(mode == 'w' || mode == 't') {
		double noteDuration = .5; // in seconds
		int numNotes = 32;
		double duration = noteDuration * numNotes;
		int dataSize = 2 * (int)(44100 * duration);
		int waveFd = makeWaveFile(argv[2],dataSize/2);
		writeSineWave(waveFd,noteDuration,261.626); // C
		writeSineWave(waveFd,noteDuration,293.665); // D
		writeSineWave(waveFd,noteDuration,329.628); // E
		writeSineWave(waveFd,4.*noteDuration/5.,261.626); // C
		writeSineWave(waveFd,noteDuration/5.,0); // Brief pause to separate C's
		writeSineWave(waveFd,noteDuration,261.626); // C
		writeSineWave(waveFd,noteDuration,293.665); // D
		writeSineWave(waveFd,noteDuration,329.628); // E 
		writeSineWave(waveFd,4.*noteDuration/5.,261.626); // C
		writeSineWave(waveFd,noteDuration/5.,0); // Brief pause
		writeSineWave(waveFd,noteDuration,329.628); // E
		writeSineWave(waveFd,noteDuration,349.228); // F
		writeSineWave(waveFd,3.*noteDuration/2.,391.995); // G
		writeSineWave(waveFd,noteDuration/2.,0); // eighth rest
		writeSineWave(waveFd,noteDuration,329.628); // E
		writeSineWave(waveFd,noteDuration,349.228); // F
		writeSineWave(waveFd,3.*noteDuration/2.,391.995); // G
		writeSineWave(waveFd,noteDuration/2.,0); // eighth rest
		writeSineWave(waveFd,noteDuration/2.,391.995); // G
		writeSineWave(waveFd,noteDuration/2.,440.); // A
		writeSineWave(waveFd,noteDuration/2.,391.995); // G
		writeSineWave(waveFd,noteDuration/2.,349.228); // F
		writeSineWave(waveFd,noteDuration,329.628); // E
		writeSineWave(waveFd,4.*noteDuration/5.,261.626); // C
		writeSineWave(waveFd,noteDuration/5.,0); // Brief pause
		writeSineWave(waveFd,noteDuration/2.,391.995); // G
		writeSineWave(waveFd,noteDuration/2.,440.); // A
		writeSineWave(waveFd,noteDuration/2.,391.995); // G
		writeSineWave(waveFd,noteDuration/2.,349.228); // F
		writeSineWave(waveFd,noteDuration,329.628); // E
		writeSineWave(waveFd,4.*noteDuration/5.,261.626); // C
		writeSineWave(waveFd,noteDuration/5.,0); // Brief pause
		writeSineWave(waveFd,noteDuration,261.626); // C
		writeSineWave(waveFd,noteDuration,195.998); // G
		writeSineWave(waveFd,noteDuration,261.626); // C
		writeSineWave(waveFd,noteDuration,0); // quarter rest
		writeSineWave(waveFd,noteDuration,261.626); // C
		writeSineWave(waveFd,noteDuration,195.998); // G
		writeSineWave(waveFd,noteDuration,261.626); // C
		writeSineWave(waveFd,noteDuration,0); // quarter rest
		close(waveFd);
	}

	if(mode == 'r' || mode == 't') {
		WaveFile data = readWaveFile(argv[2]);
		printf("id: %s, size: %d, format: %s\n", data.chunkId, data.chunkSize, data.format);
		printf("id: %s, size: %d, aFormat: %d, nChann: %d, sRate: %d, bRate: %d, bAlign: %d, bps: %d\n", data.subChunk1Id, data.subChunk1Size, data.audioFormat, data.numChannels, data.sampleRate, data.byteRate, data.blockAlign, data.bitsPerSample);
		printf("id: %s, size: %d\n", data.subChunk2Id, data.subChunk2Size);
		//readData(data.data, data.subChunk2Size);
	}

}

int makeWaveFile(char* filename, int numSamples) {
	int fd = open(filename, O_WRONLY | O_EXCL | O_CREAT, S_IRWXU);
	if(fd < 0 && errno == 17) {
		//File already exists
		printf("File \"%s\" already exists.\n", filename);
		return -1;
	} else if(fd < 0) {
		printf("Unknown write error #%d\n", errno);
		return -1;
	}
	char chunkId[4] = {'R','I','F','F'};
	char format[4] = {'W','A','V','E'};
	char chunkId1[4] = {'f','m','t',' '};
	char chunkId2[4] = {'d','a','t','a'};
	int fmtSize= 16;
	short audioFormat = 0x0001; // Linear pulse code modulation (see http://www.digitalpreservation.gov/formats/fdd/fdd000002.shtml)
	int sampleRate = 44100;
	short numChannels = 1;
	short bitsPerSample = 16;
	short blockAlign = numChannels * bitsPerSample / 8;
	int byteRate = sampleRate * blockAlign;
	int dataSize = numSamples * blockAlign;
	int size = 36 + dataSize;
	write(fd,chunkId,4);
	write(fd,&size,4);
	write(fd,format,4);
	write(fd,chunkId1,4);
	write(fd,&fmtSize,4);
	write(fd,&audioFormat,2);
	write(fd,&numChannels,2);
	write(fd,&sampleRate,4);
	write(fd,&byteRate,4);
	write(fd,&blockAlign,2);
	write(fd,&bitsPerSample,2);
	write(fd,&chunkId2,4);
	write(fd,&dataSize,4);
	return fd;
}

WaveFile readWaveFile(char* filename) {
	WaveFile result;
	int fd = open(filename, O_RDONLY);
	if(fd < 0) {
		printf("Unknown read error #%d\n", errno);
		exit(2);
	}

	read(fd,&result.chunkId,4);
	result.chunkId[4] = 0;
	read(fd,&result.chunkSize,4);
	read(fd,&result.format,4);
	result.format[4] = 0;
	read(fd,&result.subChunk1Id,4);
	result.subChunk1Id[4] = 0;
	read(fd,&result.subChunk1Size,4);
	if(result.subChunk1Size < 16) {
		printf("Error: Subchunk 1 must be at least 16 bytes\n");
		exit(3);
	}
	result.audioFormat = 0;
	read(fd,&result.audioFormat,2);
	result.numChannels = 0;
	read(fd,&result.numChannels,2);
	read(fd,&result.sampleRate,4);
	read(fd,&result.byteRate,4);
	result.blockAlign = 0;
	read(fd,&result.blockAlign,2);
	result.bitsPerSample = 0;
	read(fd,&result.bitsPerSample,2);
	read(fd,&result.subChunk2Id,4);
	result.subChunk2Id[4] = 0;
	read(fd,&result.subChunk2Size,4);
	result.data = fd;
	return result;
}

void writeSineWave(int waveFd, double duration, double frequency) {
	int samples = (int)(44100 * duration);
	short val = 0; 
	int i = 0;
	for(i = 0; i < samples; i++) {
		val = z_sine(frequency, i);
		write(waveFd,&val,2);
	}
}

short int z_sine(double f, int t) {
	return (short)(MAX_AMP * sin(2.*M_PI*f*(double)t/44100.) + 0.5);
}

void readData(int fd, int size) {
	char a[2];
	for(int i = 0; i < size; i += 2) {
		read(fd, &a, 2);
		printf("Little endian: %d, big endian: %d\n", a[0] + 256*a[1], a[1]+256*a[0]);
	}
}
