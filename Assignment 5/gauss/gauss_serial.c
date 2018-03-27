// row1 = (row2*ratio) + row1
static void subtract_rows_with_ratio(float **system, float ratio, int row1, int row2, int n){
	for(int col = 0; col < n + 1; col++){ // note n + 1
		system[row1][col] = (system[row2][col] * ratio) + system[row1][col];
	}
}

static void do_gaussian_elimination(float **system, const int n){
	int row, col;
	for(col = 0; col < n; col++){
		for(row = col + 1; row < n; row++){
			float ratio = (system[row][col] / system[col][col]) * -1.0;
			subtract_rows_with_ratio(system, ratio, row, col, n);
		}
	}
}

static void do_back_substituion(float **system, const int n){
	int row, col;
	for(col = n - 1; col > 0; col--){
		for(row = col - 1; row >= 0; row--){
			float ratio = (system[row][col] / system[col][col]) * -1.0;
			subtract_rows_with_ratio(system, ratio, row, col, n);
		}
	}
	// Normalise - everything on the left should be 1.0
	for(row = 0; row < n; row++){
		if(system[row][row] != 1.0f){
			float temp = system[row][row];
			system[row][row] /= temp;
			system[row][n] /= temp;
		}
	}
}

void solve_serial(float **system, const int n){
	do_gaussian_elimination(system, n);
	do_back_substituion(system, n);
}
