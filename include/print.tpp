/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.tpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 15:16:05 by ilbendib          #+#    #+#             */
/*   Updated: 2024/12/16 19:49:20 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_TPP
# define PRINT_TPP

template <typename T>
void printVector(std::vector<T> v)
{
	for (typename std::vector<T>::iterator it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
};

template <typename T>
void printMap(std::map<T, T> m)
{
	for (typename std::map<T, T>::iterator it = m.begin(); it != m.end(); ++it)
	{
		std::cout << it->first << " => " << it->second << std::endl;
	}
};


template <typename T>
std::string toString(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif